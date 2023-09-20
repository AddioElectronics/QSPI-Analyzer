#include "QSpiSimulationDataGenerator.h"
#include "QSpiAnalyzerSettings.h"
#include "QSpiUtil.h"

QSpiSimulationDataGenerator::QSpiSimulationDataGenerator()
{
    
}

QSpiSimulationDataGenerator::~QSpiSimulationDataGenerator()
{
}

void QSpiSimulationDataGenerator::Initialize(U32 simulation_sample_rate, QSpiAnalyzerSettings *settings)
{
    mSimulationSampleRateHz = simulation_sample_rate;
    mSettings = settings;

    mClockGenerator.Init(simulation_sample_rate / 200, simulation_sample_rate);

    mData0 = mQSpiSimulationChannels.Add(settings->mData0Channel, mSimulationSampleRateHz, BIT_LOW);
    mData1 = mQSpiSimulationChannels.Add(settings->mData1Channel, mSimulationSampleRateHz, BIT_LOW);
    mData2 = mQSpiSimulationChannels.Add(settings->mData2Channel, mSimulationSampleRateHz, BIT_HIGH);
    mData3 = mQSpiSimulationChannels.Add(settings->mData3Channel, mSimulationSampleRateHz, BIT_HIGH);
    mSck = mQSpiSimulationChannels.Add(settings->mSckChannel, mSimulationSampleRateHz, mSettings->mClockIdle);
    mCs = mQSpiSimulationChannels.Add(settings->mCsChannel, mSimulationSampleRateHz, Invert(mSettings->mEnableActiveState));


    mQSpiSimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(10.0));     //insert 10 bit-periods of idle

    mQSpiChannelManager.Init(mData0, mData1, mData2, mData3, mSettings->mShiftOrder);

    mValue = 0;
}

U32 QSpiSimulationDataGenerator::GenerateSimulationData(U64 largest_sample_requested, U32 sample_rate, SimulationChannelDescriptor **simulation_channels)
{
    U64 adjusted_largest_sample_requested = AnalyzerHelpers::AdjustSimulationTargetSample(largest_sample_requested, sample_rate, mSimulationSampleRateHz);

    while (mSck->GetCurrentSampleNumber() < adjusted_largest_sample_requested) {
		CreateQSpiTransaction();

		mData0->TransitionIfNeeded(BIT_LOW);
		mData1->TransitionIfNeeded(BIT_LOW);
		mData2->TransitionIfNeeded(BIT_HIGH);
		mData3->TransitionIfNeeded(BIT_HIGH);

        mSck->TransitionIfNeeded((BitState)mSettings->mClockIdle);

        mQSpiSimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(20.0));  //insert 20 bit-periods of idle
    }

    *simulation_channels = mQSpiSimulationChannels.GetArray();
    return mQSpiSimulationChannels.GetCount();
}

void QSpiSimulationDataGenerator::CreateQSpiTransaction()
{
    if (mCs != NULL) {
        mCs->Transition();
    }

    if (mSettings->mCsPreCycles > 0)
    {
        mQSpiSimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(mSettings->mCsPreCycles));
    }

    if (mSettings->mCommandBits > 0) {
        OutputBits(0xCDCD, mSettings->mCommandBits, GetIOMode(mSettings, QSpiTypes::COMMAND_STATE), mSettings->mDataValidEdge);
    }

    if (mSettings->mAddressBits > 0) {
        OutputBits(0xADADADADADADADAD, mSettings->mAddressBits, GetIOMode(mSettings, QSpiTypes::ADDRESS_STATE), mSettings->mDataValidEdge);
    }

    if (mSettings->mDummyBits > 0) {
        OutputBits(0, mSettings->mDummyBits, QSpiTypes::SIO, mSettings->mDataValidEdge);
    }

    U32 dataBitCount = GetStateBitCount(mSettings, QSpiTypes::DATA_STATE);
    U32 dataChunks = 256 / dataBitCount;

    for (int i = 0; i < dataChunks; i++)
    {
        OutputBits(mValue, dataBitCount, GetIOMode(mSettings, QSpiTypes::DATA_STATE), mSettings->mDataValidEdge);
        mValue++;
    }

    mData2->TransitionIfNeeded(BIT_HIGH);
    mData3->TransitionIfNeeded(BIT_HIGH);

    if (mSettings->mCsPostCycles > 0) {
        mQSpiSimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(mSettings->mCsPostCycles));
    }

    if (mCs != NULL) {
        mCs->Transition();
    }
}

void QSpiSimulationDataGenerator::OutputBits(const U64 data, const  U32 bitlen, const QSpiTypes::IOMode ioMode, const  AnalyzerEnums::Edge validEdge)
{
    switch (validEdge)
    {
        case AnalyzerEnums::LeadingEdge:
            OutputBits_CPHA0(data, bitlen, ioMode);
            break;
        case AnalyzerEnums::TrailingEdge:
            OutputBits_CPHA1(data, bitlen, ioMode);
            break;
    }
}

void QSpiSimulationDataGenerator::OutputBits_CPHA0(const U64 data, const U32 bitlen, const QSpiTypes::IOMode ioMode)
{
    BitExtractor bits(data, mSettings->mShiftOrder, bitlen);

    mQSpiChannelManager.SetIOMode(ioMode);

    for (U32 i = 0; i < bitlen / ioMode; i++) {

        for (U32 b = 0; b < ioMode; b++) {
            mQSpiChannelManager.NextChannel()->TransitionIfNeeded(bits.GetNextBit());
        }
        
        mQSpiSimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(.5));
        mSck->Transition();  //data valid

        mQSpiSimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(.5));
        mSck->Transition();  //data invalid
    }
}

void QSpiSimulationDataGenerator::OutputBits_CPHA1(const U64 data, const U32 bitlen, const QSpiTypes::IOMode ioMode)
{
    BitExtractor bits(data, mSettings->mShiftOrder, bitlen);

    mQSpiChannelManager.SetIOMode(ioMode);

    for (U32 i = 0; i < bitlen / ioMode; i++) {
        mSck->Transition();  //data invalid

        for (U32 b = 0; b < ioMode; b++) {
            mQSpiChannelManager.NextChannel()->TransitionIfNeeded(bits.GetNextBit());
        }

        mQSpiSimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(.5));
        mSck->Transition();  //data valid

        mQSpiSimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(.5));
    }
}
