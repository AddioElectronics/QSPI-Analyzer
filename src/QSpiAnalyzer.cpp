#include "QSpiAnalyzer.h"
#include "QSpiAnalyzerSettings.h"
#include <AnalyzerChannelData.h>
#include "QSpiUtil.h"

QSpiAnalyzer::QSpiAnalyzer()
    : ANALYZER_VERSION(),
        mSettings(new QSpiAnalyzerSettings()),
        mSimulationInitilized(false),
        mData0(NULL),
        mData1(NULL),
        mData2(NULL),
        mData3(NULL),
        mSck(NULL),
        mCs(NULL)
{
    SetAnalyzerSettings(mSettings.get()); 
}

QSpiAnalyzer::~QSpiAnalyzer()
{
    KillThread();
}

void QSpiAnalyzer::SetupResults()
{
    mResults.reset(new QSpiAnalyzerResults(this, mSettings.get()));
    SetAnalyzerResults(mResults.get());

    mResults->AddChannelBubblesWillAppearOn(mSettings->mData0Channel);
}

void QSpiAnalyzer::WorkerThread()
{
    Setup();

    mResults->CommitPacketAndStartNewPacket();
    mResults->CommitResults();

    //CS null not supported at this time
    if (mCs != NULL) {
        if (mCs->GetBitState() != mSettings->mEnableActiveState) {
            mCs->AdvanceToNextEdge();
        }

        mCurrentSample = mCs->GetSampleNumber();
        mSck->AdvanceToAbsPosition(mCurrentSample);
        MarkChannels(mCs->GetSampleNumber(), AnalyzerResults::Start, mStartStopMask);
    } else {
        mCurrentSample = mSck->GetSampleNumber();
    }


    for (; ;) {
        if (IsInitialClockPolarityCorrect() == true) { //if false, this function moves to the next active enable edge.
            break;
        }
    }

    for (; ;) {

        switch (mTransactionState)
        {
            case QSpiTypes::SETUP_STATE:
                AdvanceState();
            break;
            case QSpiTypes::DUMMY_STATE:
                GetDummyBlock();
                break;
            case QSpiTypes::ADDRESS_STATE:
            case QSpiTypes::COMMAND_STATE:
            case QSpiTypes::DATA_STATE:
                GetBlock();
                break;
        }

        CheckIfThreadShouldExit();
    }
}

void QSpiAnalyzer::AdvanceToActiveEnableEdgeWithCorrectClockPolarity()
{
    mResults->CommitPacketAndStartNewPacket();
    mResults->CommitResults();

    AdvanceToActiveEnableEdge();

    for (; ;) {
        if (IsInitialClockPolarityCorrect() == true) { //if false, this function moves to the next active enable edge.
            break;
        }
    }

    MarkChannels(mCs->GetSampleNumber(), AnalyzerResults::Start, mStartStopMask);
}

void QSpiAnalyzer::Setup()
{
    bool allow_last_trailing_clock_edge_to_fall_outside_enable = false;
    if (mSettings->mDataValidEdge == AnalyzerEnums::LeadingEdge) {
        allow_last_trailing_clock_edge_to_fall_outside_enable = true;
    }

    if (mSettings->mClockIdle == BIT_LOW) {
        if (mSettings->mDataValidEdge == AnalyzerEnums::LeadingEdge) {
            mArrowMarker = AnalyzerResults::UpArrow;
        } else {
            mArrowMarker = AnalyzerResults::DownArrow;
        }

    } else {
        if (mSettings->mDataValidEdge == AnalyzerEnums::LeadingEdge) {
            mArrowMarker = AnalyzerResults::DownArrow;
        } else {
            mArrowMarker = AnalyzerResults::UpArrow;
        }
    }

    mData0 = GetAnalyzerChannelData(mSettings->mData0Channel);
    mData1 = GetAnalyzerChannelData(mSettings->mData1Channel);
    mData2 = GetAnalyzerChannelData(mSettings->mData2Channel);
    mData3 = GetAnalyzerChannelData(mSettings->mData3Channel);

    mSck = GetAnalyzerChannelData(mSettings->mSckChannel);
    mCs = GetAnalyzerChannelData(mSettings->mCsChannel);

    mQSpiChannelManager.Init(mData0, mData1, mData2, mData3, mSettings->mShiftOrder);

    mTransactionState = QSpiTypes::SETUP_STATE;
    AdvanceState();

    
}

void QSpiAnalyzer::AdvanceToActiveEnableEdge()
{
    if (mCs != NULL) {
        if (mCs->GetBitState() != mSettings->mEnableActiveState) {
            mCs->AdvanceToNextEdge();
        } else {
            mCs->AdvanceToNextEdge();
            mCs->AdvanceToNextEdge();
        }
        mCurrentSample = mCs->GetSampleNumber();
        mSck->AdvanceToAbsPosition(mCurrentSample - 1);
    } else {
        mCurrentSample = mSck->GetSampleNumber();
    }


    mTransactionState = QSpiTypes::SETUP_STATE;
    AdvanceState();
}

bool QSpiAnalyzer::IsInitialClockPolarityCorrect()
{
    if (mSck->GetBitState() == mSettings->mClockIdle) {
        return true;
    }

    if (mSettings->mShowMarker) {
        mResults->AddMarker(mCurrentSample, AnalyzerResults::ErrorSquare, mSettings->mSckChannel);
    }

    if (mCs != NULL) {
        Frame error_frame;
        error_frame.mStartingSampleInclusive = mCurrentSample;

        mCs->AdvanceToNextEdge();
        mCurrentSample = mCs->GetSampleNumber();

        error_frame.mEndingSampleInclusive = mCurrentSample;
        error_frame.mFlags = QSPI_ERROR_FLAG | DISPLAY_AS_ERROR_FLAG;
        mResults->AddFrame(error_frame);
        mResults->CommitResults();
        ReportProgress(error_frame.mEndingSampleInclusive);

        //move to the next active-going enable edge
        mCs->AdvanceToNextEdge();
        mCurrentSample = mCs->GetSampleNumber();
        mSck->AdvanceToAbsPosition(mCurrentSample);

        return false;
    } else {
        mSck->AdvanceToNextEdge();  //at least start with the clock in the idle state.
        mCurrentSample = mSck->GetSampleNumber();
        return true;
    }
}

bool QSpiAnalyzer::WouldAdvancingTheClockToggleEnable()
{
    if (mCs == NULL) {
        return false;
    }

    U64 next_edge = mSck->GetSampleOfNextEdge();
    bool enable_will_toggle = mCs->WouldAdvancingToAbsPositionCauseTransition(next_edge);

    return enable_will_toggle;
}


void QSpiAnalyzer::AdvanceState()
{
    mTransactionState = GetNextState(mSettings.get(), mTransactionState);
    mQSpiChannelManager.SetIOMode(GetIOMode(mSettings.get(), mTransactionState));
}


void QSpiAnalyzer::GetBlock()
{
    QSpiTypes::IOMode ioMode = GetIOMode(mSettings.get(), mTransactionState);
    U32 bits_per_transfer = GetStateBitCount(mSettings.get(), mTransactionState);
    U32 clocks_per_segment = GetStateClockCount(mSettings.get(), mTransactionState, ioMode);

    DataBuilder data_result;
    U64 data_word;
    data_result.Reset(&data_word, mSettings->mShiftOrder, bits_per_transfer);

    U64 first_sample = 0;
    bool need_reset = false;
    bool data_state_complete = false;

    mArrowLocations.clear();
    ReportProgress(mSck->GetSampleNumber());

    for (U32 i = 0; i < clocks_per_segment; i++) {
        //on every single edge, we need to check that enable doesn't toggle.
        //note that we can't just advance the enable line to the next edge, becuase there may not be another edge
        if (WouldAdvancingTheClockToggleEnable() == true) {

            //Last data chunk
            if (mTransactionState == QSpiTypes::DATA_STATE && i > 0) {
                data_state_complete = true;
                need_reset = true;
                break;
            }

            MarkChannels(mCs->GetSampleOfNextEdge(), AnalyzerResults::Stop, mStartStopMask);
            AdvanceToActiveEnableEdgeWithCorrectClockPolarity();  //ok, we pretty much need to reset everything and return.
            return;
        }

        mSck->AdvanceToNextEdge();
        if (i == 0) {
            first_sample = mSck->GetSampleNumber();
            /*if (mSettings->mShowMarker) {
                if (mSettings->mDataValidEdge == AnalyzerEnums::LeadingEdge) {
                    mResults->AddMarker(mSck->GetSampleOfNextEdge(), AnalyzerResults::Start, mSettings->mSckChannel);
                }
                else {
                    mResults->AddMarker(first_sample, AnalyzerResults::Start, mSettings->mSckChannel);
                }
            }*/
        }

        if (mSettings->mDataValidEdge == AnalyzerEnums::LeadingEdge) {
            mCurrentSample = mSck->GetSampleNumber();

            for (U32 i = 0; i < ioMode; i++)
            {
                AnalyzerChannelData* channel = mQSpiChannelManager.NextChannel();
                channel->AdvanceToAbsPosition(mCurrentSample);
                data_result.AddBit(channel->GetBitState());
            }

            mArrowLocations.push_back(mCurrentSample);
        }

        // ok, the trailing edge is messy -- but only on the very last bit.
        // If the trialing edge isn't doesn't represent valid data, we want to allow the enable line to rise before the clock trialing edge -- and still report the frame
        if ((i == (clocks_per_segment - 1)) && (mSettings->mDataValidEdge != AnalyzerEnums::TrailingEdge)) {
            //if this is the last bit, and the trailing edge doesn't represent valid data
            if (WouldAdvancingTheClockToggleEnable() == true) {
                //moving to the trailing edge would cause the clock to revert to inactive.  jump out, record the frame, and them move to the next active enable edge
                need_reset = true;

                //Last data chunk
                if (mTransactionState == QSpiTypes::DATA_STATE) {
                    data_state_complete = true;
                }
                break;
            }

            //enable isn't going to go inactive, go ahead and advance the clock as usual.  Then we're done, jump out and record the frame.
            mSck->AdvanceToNextEdge();
            break;
        }

        //this isn't the very last bit, etc, so proceed as normal
        if (WouldAdvancingTheClockToggleEnable() == true) {

            //Last data chunk
            if (mTransactionState == QSpiTypes::DATA_STATE) {
                data_state_complete = true;
                need_reset = true;
                break;
            }

            MarkChannels(mCs->GetSampleOfNextEdge(), AnalyzerResults::Stop, mStartStopMask);

            AdvanceToActiveEnableEdgeWithCorrectClockPolarity();  //ok, we pretty much need to reset everything and return.
            return;
        }

        mSck->AdvanceToNextEdge();

        if (mSettings->mDataValidEdge == AnalyzerEnums::TrailingEdge) {
            mCurrentSample = mSck->GetSampleNumber();

            for (U32 i = 0; i < ioMode; i++)
            {
                AnalyzerChannelData* channel = mQSpiChannelManager.NextChannel();
                channel->AdvanceToAbsPosition(mCurrentSample);
                data_result.AddBit(channel->GetBitState());
            }
            mArrowLocations.push_back(mCurrentSample);
        }

    }

    bool advancing = mTransactionState != QSpiTypes::DATA_STATE || data_state_complete;

    //save the resuls:
    U32 count = mArrowLocations.size();
    for (U32 i = 0; i < count; i++) {
        if (mSettings->mShowMarker) {
            mResults->AddMarker(mArrowLocations[i], mArrowMarker, mSettings->mSckChannel);
            U8 dataMask = GetChannelMask(mSettings.get(), mTransactionState);
            U8 unusedMask = ~dataMask & 0b1111;
            MarkChannels(mArrowLocations[i], AnalyzerResults::Dot, dataMask);
            MarkChannels(mArrowLocations[i], AnalyzerResults::X, unusedMask);
        }
    }

    Frame result_frame;
    result_frame.mStartingSampleInclusive = first_sample;
    result_frame.mEndingSampleInclusive = WouldAdvancingTheClockToggleEnable () ? mCs->GetSampleOfNextEdge() : mSck->GetSampleOfNextEdge();
    result_frame.mData1 = data_word;
    result_frame.mFlags = 0;
    result_frame.mType = (U8)mTransactionState;

    mResults->AddFrame(result_frame);
    mResults->CommitResults();

    if (advancing){
        AdvanceState();
    }

    if (need_reset == true) {
        MarkChannels(mCs->GetSampleOfNextEdge(), AnalyzerResults::Stop, mStartStopMask);
        AdvanceToActiveEnableEdgeWithCorrectClockPolarity();
    }
}


void QSpiAnalyzer::GetDummyBlock()
{

    U32 bits_per_transfer = GetStateBitCount(mSettings.get(), QSpiTypes::DUMMY_STATE);

    U64 first_sample = 0;
    bool need_reset = false;

    mArrowLocations.clear();
    ReportProgress(mSck->GetSampleNumber());

    for (U32 i = 0; i < bits_per_transfer; i++) {
        //on every single edge, we need to check that enable doesn't toggle.
        //note that we can't just advance the enable line to the next edge, becuase there may not be another edge

        if (WouldAdvancingTheClockToggleEnable() == true) {

            if (mTransactionState == QSpiTypes::DATA_STATE)
                break;

            AdvanceToActiveEnableEdgeWithCorrectClockPolarity();  //ok, we pretty much need to reset everything and return.
            return;
        }

        mSck->AdvanceToNextEdge();
        if (i == 0) {
            first_sample = mSck->GetSampleNumber();
        }

        if (mSettings->mDataValidEdge == AnalyzerEnums::LeadingEdge) {
            mCurrentSample = mSck->GetSampleNumber();
            mArrowLocations.push_back(mCurrentSample);
        }

        // ok, the trailing edge is messy -- but only on the very last bit.
        // If the trialing edge isn't doesn't represent valid data, we want to allow the enable line to rise before the clock trialing edge -- and still report the frame
        if ((i == (bits_per_transfer - 1)) && (mSettings->mDataValidEdge != AnalyzerEnums::TrailingEdge)) {
            //if this is the last bit, and the trailing edge doesn't represent valid data
            if (WouldAdvancingTheClockToggleEnable() == true) {
                //moving to the trailing edge would cause the clock to revert to inactive.  jump out, record the frame, and them move to the next active enable edge
                return;
            }

            //enable isn't going to go inactive, go ahead and advance the clock as usual.  Then we're done, jump out and record the frame.
            mSck->AdvanceToNextEdge();
            break;
        }

        //this isn't the very last bit, etc, so proceed as normal
        if (WouldAdvancingTheClockToggleEnable() == true) {
            AdvanceToActiveEnableEdgeWithCorrectClockPolarity();  //ok, we pretty much need to reset everything and return.
            return;
        }

        mSck->AdvanceToNextEdge();

        if (mSettings->mDataValidEdge == AnalyzerEnums::TrailingEdge) {
            mCurrentSample = mSck->GetSampleNumber();
            mArrowLocations.push_back(mCurrentSample);
        }

    }

    //save the resuls:
    U32 count = mArrowLocations.size();
    for (U32 i = 0; i < count; i++) {
        if (mSettings->mShowMarker) {
            mResults->AddMarker(mArrowLocations[i], mArrowMarker, mSettings->mSckChannel);
            //mResults->AddMarker(mArrowLocations[i], AnalyzerResults::X, mSettings->mData0Channel);
            MarkChannels(mArrowLocations[i], AnalyzerResults::X, 0b1111);
        }
    }

    Frame result_frame;
    result_frame.mStartingSampleInclusive = first_sample;
    result_frame.mEndingSampleInclusive = WouldAdvancingTheClockToggleEnable() ? mSck->GetSampleNumber() : mSck->GetSampleOfNextEdge();
    result_frame.mData1 = 0;
    result_frame.mFlags = 0;
    result_frame.mType = (U8)mTransactionState;

    mResults->AddFrame(result_frame);
    mResults->CommitResults();
    AdvanceState();

    if (need_reset == true) {
        AdvanceToActiveEnableEdgeWithCorrectClockPolarity();
    }
}

void QSpiAnalyzer::MarkChannels(const U64 sampleNum, const AnalyzerResults::MarkerType marker, const U32 channelMask)
{
   /* if (!mSettings->mShowMarker)
        return;*/

    for (int i = 0; i < 6; i++)
    {
        if (!(channelMask >> i & 1))
            continue;

        mResults->AddMarker(sampleNum, marker, *mSettings->mChannels[i]);
    }
}




bool QSpiAnalyzer::NeedsRerun()
{
    return false;
}

U32 QSpiAnalyzer::GenerateSimulationData(U64 minimum_sample_index, U32 device_sample_rate, SimulationChannelDescriptor **simulation_channels)
{
    if (mSimulationInitilized == false) {
        mSimulationDataGenerator.Initialize(GetSimulationSampleRate(), mSettings.get());
        mSimulationInitilized = true;
    }

    return mSimulationDataGenerator.GenerateSimulationData(minimum_sample_index, device_sample_rate, simulation_channels);
}


U32 QSpiAnalyzer::GetMinimumSampleRateHz()
{
    return 10000; //we don't have any idea, depends on the QSPI rate, etc.; return the lowest rate.
}

const char *QSpiAnalyzer::GetAnalyzerName() const
{
    return "QSPI";
}

const char *GetAnalyzerName()
{
    return "QSPI";
}

Analyzer *CreateAnalyzer()
{
    return new QSpiAnalyzer();
}

void DestroyAnalyzer(Analyzer *analyzer)
{
    delete analyzer;
}
