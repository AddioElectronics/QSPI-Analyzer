#include <AnalyzerHelpers.h>
#include "QSpiAnalyzer.h"
#include "QSpiAnalyzerSettings.h"
#include "QSpiAnalyzerResults.h"
#include "QSpiUtil.h"
#include <iostream>
#include <sstream>

#pragma warning(disable: 4996) //warning C4996: 'sprintf': This function or variable may be unsafe. Consider using sprintf_s instead.

QSpiAnalyzerResults::QSpiAnalyzerResults(QSpiAnalyzer *analyzer, QSpiAnalyzerSettings *settings)
    :   AnalyzerResults(),
        mSettings(settings),
        mAnalyzer(analyzer)
{
}

QSpiAnalyzerResults::~QSpiAnalyzerResults()
{
}

void QSpiAnalyzerResults::GenerateBubbleText(U64 frame_index, Channel &channel, DisplayBase display_base)    //unrefereced vars commented out to remove warnings.
{
    ClearResultStrings();
    Frame frame = GetFrame(frame_index);

    if ((frame.mFlags & QSPI_ERROR_FLAG) == 0) {
        if (channel == mSettings->mData0Channel) {

            QSpiTypes::TransactionState transaction_state = (QSpiTypes::TransactionState)frame.mType;
            QSpiTypes::IOMode data_lines = GetIOMode(mSettings, transaction_state);
            U32 bit_count = GetStateBitCount(mSettings, transaction_state);
            U32 clock_count = GetStateClockCount(mSettings, transaction_state, data_lines);

            char number_str[128];
            char result_str[128];

            if (frame.mType == QSpiTypes::DUMMY_STATE)
            {
                AddResultString("X");
                AddResultString("Dummy");
                data_index = 0;
            }
            else
            {
                AnalyzerHelpers::GetNumberString(frame.mData1, display_base, bit_count, number_str, sizeof(number_str));

                switch (frame.mType)
                {
                case QSpiTypes::COMMAND_STATE:
                    AddResultString("C");
                    AddResultString("CMD");
                    snprintf(result_str, sizeof(result_str), "CMD: %s", number_str);
                    data_index = 0;
                    break;
                case QSpiTypes::ADDRESS_STATE:
                    AddResultString("A");
                    AddResultString("Addr");
                    snprintf(result_str, sizeof(result_str), "Addr: %s", number_str);
                    data_index = 0;
                    break;
                case QSpiTypes::DATA_STATE:
                    AddResultString("D");
                    //AddResultString("Data");
                    AddResultString(number_str);
                    snprintf(result_str, sizeof(result_str), "Data: %s", number_str);
                    AddResultString(result_str);
                    snprintf(result_str, sizeof(result_str), "Data %d: %s", data_index++, number_str);
                    break;
                }

                
                
            }
            AddResultString(result_str);
        }
    } else {
        AddResultString("Error");
        AddResultString("Settings mismatch");
        AddResultString("The initial (idle) state of the CLK line does not match the settings.");
    }
}

void QSpiAnalyzerResults::GenerateExportFile(const char *file, DisplayBase display_base, U32 /*export_type_user_id*/)
{
    //export_type_user_id is only important if we have more than one export type.

    std::stringstream ss;
    void *f = AnalyzerHelpers::StartFile(file);

    U64 trigger_sample = mAnalyzer->GetTriggerSample();
    U32 sample_rate = mAnalyzer->GetSampleRate();

    ss << "Time [s],Packet ID, Transaction State, DATA, Lines Used" << std::endl;

    U64 num_frames = GetNumFrames();
    for (U32 i = 0; i < num_frames; i++) {
        Frame frame = GetFrame(i);

        if ((frame.mFlags & QSPI_ERROR_FLAG) != 0) {
            continue;
        }

        char time_str[128];
        AnalyzerHelpers::GetTimeString(frame.mStartingSampleInclusive, trigger_sample, sample_rate, time_str, 128);

        char data_str[128] = "";

        QSpiTypes::TransactionState transaction_state = (QSpiTypes::TransactionState)frame.mType;
        QSpiTypes::IOMode data_lines = GetIOMode(mSettings, transaction_state);
        U32 bit_count = GetStateBitCount(mSettings, transaction_state);
        U32 clock_count = GetStateClockCount(mSettings, transaction_state, data_lines);

        if (frame.mType == QSpiTypes::DUMMY_STATE){
            strcpy(data_str, "Dummy");
        }
        else{
            AnalyzerHelpers::GetNumberString(frame.mData1, display_base, bit_count, data_str, 128);
        }

        U64 packet_id = GetPacketContainingFrameSequential(i);
        if (packet_id != INVALID_RESULT_INDEX) {
            ss << time_str << "," << packet_id << "," << (U32)frame.mType << "," << data_str << "," << data_lines << std::endl;
        } else {
            ss << time_str << ",," << (U32)frame.mType << "," << data_str << "," << data_lines << std::endl;    //it's ok for a frame not to be included in a packet.
        }

        AnalyzerHelpers::AppendToFile((U8 *)ss.str().c_str(), ss.str().length(), f);
        ss.str(std::string());

        if (UpdateExportProgressAndCheckForCancel(i, num_frames) == true) {
            AnalyzerHelpers::EndFile(f);
            return;
        }
    }

    UpdateExportProgressAndCheckForCancel(num_frames, num_frames);
    AnalyzerHelpers::EndFile(f);
}

void QSpiAnalyzerResults::GenerateFrameTabularText(U64 frame_index, DisplayBase display_base)
{
    ClearTabularText();
    Frame frame = GetFrame(frame_index);


    char data_str[128];
    QSpiTypes::TransactionState transaction_state = (QSpiTypes::TransactionState)frame.mType;
    QSpiTypes::IOMode data_lines = GetIOMode(mSettings, transaction_state);
    U32 bit_count = GetStateBitCount(mSettings, transaction_state);
    U32 clock_count = GetStateClockCount(mSettings, transaction_state, data_lines);

    std::stringstream ss;

    if ((frame.mFlags & QSPI_ERROR_FLAG) == 0) {

        if (frame.mType != QSpiTypes::DATA_STATE && data_index > 0)
        {
            data_index = 0;
            /*ss << " ";
            AddTabularText(ss.str().c_str());
            ss.clear();*/
        }

        if (frame.mType == QSpiTypes::DUMMY_STATE)
        {
            /*ss << "Dummy: ";
            strcpy(data_str, "(Dummy)");*/
            sprintf(data_str, "(Dummy) %d bits", bit_count);
        }
        else
        {
            switch (frame.mType)
            {
                case QSpiTypes::COMMAND_STATE:
                    ss << "CMD: ";
                    break;
                case QSpiTypes::ADDRESS_STATE:
                    ss << "Addr: ";
                    break;
                case QSpiTypes::DATA_STATE:
                    sprintf(data_str, "Data %d: ", data_index++);
                    //ss << "Data: ";
                    ss << data_str;
                    break;
            }

            AnalyzerHelpers::GetNumberString(frame.mData1, display_base, bit_count, data_str, 128);
        }

        ss << data_str;

    } else {
        ss << "The initial (idle) state of the CLK line does not match the settings.";
    }

    AddTabularText(ss.str().c_str());
}

void QSpiAnalyzerResults::GeneratePacketTabularText(U64 /*packet_id*/, DisplayBase /*display_base*/)    //unrefereced vars commented out to remove warnings.
{
    ClearResultStrings();
    AddResultString("not supported");
}

void QSpiAnalyzerResults::GenerateTransactionTabularText(U64 /*transaction_id*/, DisplayBase /*display_base*/)    //unrefereced vars commented out to remove warnings.
{
    ClearResultStrings();
    AddResultString("not supported");
}
