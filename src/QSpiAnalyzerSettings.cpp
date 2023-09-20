#include "QSpiAnalyzerSettings.h"

#include <AnalyzerHelpers.h>
#include <sstream>
#include <cstring>


QSpiAnalyzerSettings::QSpiAnalyzerSettings()
    :   
        mData0Channel(UNDEFINED_CHANNEL),
        mData1Channel(UNDEFINED_CHANNEL),
        mData2Channel(UNDEFINED_CHANNEL),
        mData3Channel(UNDEFINED_CHANNEL),
        mSckChannel(UNDEFINED_CHANNEL),
        mCsChannel(UNDEFINED_CHANNEL),

        mShiftOrder(AnalyzerEnums::MsbFirst),
        mClockIdle(BIT_HIGH),
        mDataValidEdge(AnalyzerEnums::TrailingEdge),
        mEnableActiveState(BIT_LOW),

        mCommandBits(8),
        mAddressBits(8),
        mDummyBits(0),
        mDataBits(8),

        mCommandIOMode(QSpiTypes::SIO),
        mAddressIOMode(QSpiTypes::QIO),
        mDataIOMode(QSpiTypes::QIO),

        mCsPreCycles(0),
        mCsPostCycles(0),
        mShowMarker(BIT_HIGH)
{
    mData0ChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
    mData0ChannelInterface->SetTitleAndTooltip("DQ0", "Data 0");
    mData0ChannelInterface->SetChannel(mData0Channel);

    mData1ChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
    mData1ChannelInterface->SetTitleAndTooltip("DQ1", "Data 1");
    mData1ChannelInterface->SetChannel(mData1Channel);

    mData2ChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
    mData2ChannelInterface->SetTitleAndTooltip("DQ2", "Data 2");
    mData2ChannelInterface->SetChannel(mData2Channel);

    mData3ChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
    mData3ChannelInterface->SetTitleAndTooltip("DQ3", "Data 3");
    mData3ChannelInterface->SetChannel(mData3Channel);

    mSckChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
    mSckChannelInterface->SetTitleAndTooltip("SCK", "Clock");
    mSckChannelInterface->SetChannel(mSckChannel);

    mCsChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
    mCsChannelInterface->SetTitleAndTooltip("CS", "Chip Select");
    mCsChannelInterface->SetChannel(mCsChannel);


    mCommandBitsInterface.reset(new AnalyzerSettingInterfaceNumberList());
    mCommandBitsInterface->SetTitleAndTooltip("Command Bits", "Specify how many command bits");
    mCommandBitsInterface->SetNumber(mCommandBits);

    mAddressBitsInterface.reset(new AnalyzerSettingInterfaceNumberList());
    mAddressBitsInterface->SetTitleAndTooltip("Address Bits", "Specify how many address bits");
    mAddressBitsInterface->SetNumber(mAddressBits);

    mDummyBitsInterface.reset(new AnalyzerSettingInterfaceNumberList());
    mDummyBitsInterface->SetTitleAndTooltip("Dummy Bits", "Specify how many dummy bits between address and data.");
    mDummyBitsInterface->SetNumber(mDummyBits);

    mDataBitsInterface.reset(new AnalyzerSettingInterfaceNumberList());
    mDataBitsInterface->SetTitleAndTooltip("Data Bit Width", "Specify how many data bits are analyzed per frame.");
    mDataBitsInterface->SetNumber(mDataBits);

    mCsPreEnabledInterface.reset(new AnalyzerSettingInterfaceNumberList());
    mCsPreEnabledInterface->SetTitleAndTooltip("Pre CS Clocks", "Specify how many clock cycles CS will activate before transmission");
    mCsPreEnabledInterface->SetNumber(mCsPreCycles);

    mCsPostEnabledInterface.reset(new AnalyzerSettingInterfaceNumberList());
    mCsPostEnabledInterface->SetTitleAndTooltip("Post CS Clocks", "Specify how many clock cycles CS will stay active after transmission");
    mCsPostEnabledInterface->SetNumber(mCsPostCycles);

    for (int i = 0; i <= 64; i++)
    {
        char data[4];
        snprintf(data, 4, "%d", i);
        if (i < 17)
        {
            mCommandBitsInterface->AddNumber(i, data, "");
            mDummyBitsInterface->AddNumber(i, data, "");
            mCsPreEnabledInterface->AddNumber(i, data, "");
            mCsPostEnabledInterface->AddNumber(i, data, "");
        }
        mAddressBitsInterface->AddNumber(i, data, "");

        if ((i != 0 && i % 8 == 0) || i == 4) {
            mDataBitsInterface->AddNumber(i, data, "");
        }
    }

    mCommandIOModeInterface.reset(new AnalyzerSettingInterfaceNumberList());
    mCommandIOModeInterface->SetTitleAndTooltip("Command Data Lines", "Specify how many data lines the command uses");
    mCommandIOModeInterface->AddNumber(QSpiTypes::SIO, "Single", "");
    mCommandIOModeInterface->AddNumber(QSpiTypes::DIO, "Dual", "");
    mCommandIOModeInterface->AddNumber(QSpiTypes::QIO, "Quad", "");
    mCommandIOModeInterface->SetNumber(mCommandIOMode);

    mAddressIOModeInterface.reset(new AnalyzerSettingInterfaceNumberList());
    mAddressIOModeInterface->SetTitleAndTooltip("Address Data Lines", "Specify how many data lines the address uses");
    mAddressIOModeInterface->AddNumber(QSpiTypes::SIO, "Single", "");
    mAddressIOModeInterface->AddNumber(QSpiTypes::DIO, "Dual", "");
    mAddressIOModeInterface->AddNumber(QSpiTypes::QIO, "Quad", "");
    mAddressIOModeInterface->SetNumber(mAddressIOMode);

    mDataIOModeInterface.reset(new AnalyzerSettingInterfaceNumberList());
    mDataIOModeInterface->SetTitleAndTooltip("Data Data Lines", "Specify how many data lines the data uses. If not quad, command and address is force to match data.");
    mDataIOModeInterface->AddNumber(QSpiTypes::SIO, "Single", "");
    mDataIOModeInterface->AddNumber(QSpiTypes::DIO, "Dual", "");
    mDataIOModeInterface->AddNumber(QSpiTypes::QIO, "Quad", "");
    mDataIOModeInterface->SetNumber(mDataIOMode);

    mShiftOrderInterface.reset(new AnalyzerSettingInterfaceNumberList());
    mShiftOrderInterface->SetTitleAndTooltip("Endianness", "Specify which bit is transferred first");
    mShiftOrderInterface->AddNumber(AnalyzerEnums::MsbFirst, "Most Significant Bit First (Standard)", "");
    mShiftOrderInterface->AddNumber(AnalyzerEnums::LsbFirst, "Least Significant Bit First", "");
    mShiftOrderInterface->SetNumber(mShiftOrder);

    mClockIdleInterface.reset(new AnalyzerSettingInterfaceNumberList());
    mClockIdleInterface->SetTitleAndTooltip("Clock Idle", "Specify clock state when idle");
    mClockIdleInterface->AddNumber(QSpiTypes::CPOL0, "Clock is Low when inactive(CPOL = 0)", "CPOL = 0 (Clock Polarity)");
    mClockIdleInterface->AddNumber(QSpiTypes::CPOL1, "Clock is High when inactive (CPOL = 1)", "CPOL = 1 (Clock Polarity)");
    mClockIdleInterface->SetNumber(mClockIdle);

    mDataValidEdgeInterface.reset(new AnalyzerSettingInterfaceNumberList());
    mDataValidEdgeInterface->SetTitleAndTooltip("Edge Trigger", "Specify edge the data is triggered on");
    mDataValidEdgeInterface->AddNumber(AnalyzerEnums::LeadingEdge, "Data is Valid on Clock Leading Edge (CPHA = 0)", "CPHA = 0 (Clock Phase)");
    mDataValidEdgeInterface->AddNumber(AnalyzerEnums::TrailingEdge, "Data is Valid on Clock Trailing Edge (CPHA = 1)", "CPHA = 1 (Clock Phase)");
    mDataValidEdgeInterface->SetNumber(mDataValidEdge);

    mEnableActiveStateInterface.reset(new AnalyzerSettingInterfaceNumberList());
    mEnableActiveStateInterface->SetTitleAndTooltip("CS Idle", "Specify the CS line's idle state");
    mEnableActiveStateInterface->AddNumber(BIT_LOW, "Enable line is Active Low (Standard)", "");
    mEnableActiveStateInterface->AddNumber(BIT_HIGH, "Enable line is Active High", "");
    mEnableActiveStateInterface->SetNumber(mEnableActiveState);


    mDataRateInterface.reset(new AnalyzerSettingInterfaceNumberList());
    mDataRateInterface->SetTitleAndTooltip("Data Rate", "Specify the if data is clocked on a single edge, or both rising and falling.");
    mDataRateInterface->AddNumber(QSpiTypes::SDR, "SDR", "");
    mDataRateInterface->AddNumber(QSpiTypes::DDR, "DDR", "");
    mDataRateInterface->SetNumber(mDataRate);

    mUseShowMarkerInterface.reset(new AnalyzerSettingInterfaceBool());
    mUseShowMarkerInterface->SetTitleAndTooltip("", "Show decode marker or not");
    mUseShowMarkerInterface->SetCheckBoxText("Show Decode Marker");
    mUseShowMarkerInterface->SetValue(mShowMarker);

    AddInterface(mData0ChannelInterface.get());
    AddInterface(mData1ChannelInterface.get());
    AddInterface(mData2ChannelInterface.get());
    AddInterface(mData3ChannelInterface.get());
    AddInterface(mSckChannelInterface.get());
    AddInterface(mCsChannelInterface.get());

    AddInterface(mShiftOrderInterface.get());
    AddInterface(mClockIdleInterface.get());
    AddInterface(mDataValidEdgeInterface.get());
    AddInterface(mEnableActiveStateInterface.get());

    AddInterface(mCommandBitsInterface.get());
    AddInterface(mAddressBitsInterface.get());
    AddInterface(mDummyBitsInterface.get());
    AddInterface(mDataBitsInterface.get());

    AddInterface(mCommandIOModeInterface.get());
    AddInterface(mAddressIOModeInterface.get());
    AddInterface(mDataIOModeInterface.get());
    //AddInterface(mDataRateInterface.get());

    AddInterface(mCsPreEnabledInterface.get());
    AddInterface(mCsPostEnabledInterface.get());

    AddInterface(mUseShowMarkerInterface.get());

    AddExportOption(0, "Export as text/csv file");
    AddExportExtension(0, "Text file", "txt");
    AddExportExtension(0, "CSV file", "csv");

    ClearChannels();

    AddChannel(mData0Channel, "DQ0", false);
    AddChannel(mData1Channel, "DQ1", false);
    AddChannel(mData2Channel, "DQ2", false);
    AddChannel(mData3Channel, "DQ3", false);
    AddChannel(mSckChannel, "SCK", false);
    AddChannel(mCsChannel, "CS", false);
}

QSpiAnalyzerSettings::~QSpiAnalyzerSettings()
{
}

bool QSpiAnalyzerSettings::SetSettingsFromInterfaces()
{
    Channel data0 = mData0ChannelInterface->GetChannel();
    Channel data1 = mData1ChannelInterface->GetChannel();
    Channel data2 = mData2ChannelInterface->GetChannel();
    Channel data3 = mData3ChannelInterface->GetChannel();
    Channel sck = mSckChannelInterface->GetChannel();
    Channel cs = mCsChannelInterface->GetChannel();


    std::vector<Channel> _channels;
    _channels.push_back(data0);
    _channels.push_back(data1);
    _channels.push_back(data2);
    _channels.push_back(data3);
    _channels.push_back(sck);
    _channels.push_back(cs);

    if (AnalyzerHelpers::DoChannelsOverlap(&_channels[0], _channels.size()) == true) {
        SetErrorText("Please select different _channels for each input.");
        return false;
    }

    for (int i = 0; i < 6; i++)
    {
        if (_channels[i] == UNDEFINED_CHANNEL)
        {
            SetErrorText("Atleast 1 channel is undefined. Please set all _channels.");
        }

    }


    mData0Channel = mData0ChannelInterface->GetChannel();
    mData1Channel = mData1ChannelInterface->GetChannel();
    mData2Channel = mData2ChannelInterface->GetChannel();
    mData3Channel = mData3ChannelInterface->GetChannel();
    mSckChannel = mSckChannelInterface->GetChannel();
    mCsChannel = mCsChannelInterface->GetChannel();

    mShiftOrder = (AnalyzerEnums::ShiftOrder) U32(mShiftOrderInterface->GetNumber());
    mClockIdle = (BitState)(U32(mClockIdleInterface->GetNumber()));
    mDataValidEdge = AnalyzerEnums::Edge(U32(mDataValidEdgeInterface->GetNumber()));
    mEnableActiveState = (BitState) U32(mEnableActiveStateInterface->GetNumber());

    mCommandBits = mCommandBitsInterface->GetNumber();
    mAddressBits = mAddressBitsInterface->GetNumber();
    mDummyBits = mDummyBitsInterface->GetNumber();
    mDataBits = mDataBitsInterface->GetNumber();

    mCommandIOMode = QSpiTypes::IOMode(U32(mCommandIOModeInterface->GetNumber()));
    mAddressIOMode = QSpiTypes::IOMode(U32(mAddressIOModeInterface->GetNumber()));
    mDataIOMode = QSpiTypes::IOMode(U32(mDataIOModeInterface->GetNumber()));
    mDataRate = QSpiTypes::DataRate(U32(mDataRateInterface->GetNumber()));

    mCsPreCycles = mCsPreEnabledInterface->GetNumber();
    mCsPostCycles = mCsPostEnabledInterface->GetNumber();

    mShowMarker = mUseShowMarkerInterface->GetValue();

    ClearChannels();

    AddChannel(mData0Channel, "DQ0", mData0Channel != UNDEFINED_CHANNEL);
    AddChannel(mData1Channel, "DQ1", mData1Channel != UNDEFINED_CHANNEL);
    AddChannel(mData2Channel, "DQ2", mData2Channel != UNDEFINED_CHANNEL);
    AddChannel(mData3Channel, "DQ3", mData3Channel != UNDEFINED_CHANNEL);
    AddChannel(mSckChannel, "SCK", mSckChannel != UNDEFINED_CHANNEL);
    AddChannel(mCsChannel, "CS", mCsChannel != UNDEFINED_CHANNEL);

    return true;
}

void QSpiAnalyzerSettings::LoadSettings(const char *settings)
{
    SimpleArchive text_archive;
    text_archive.SetString(settings);

    const char *name_string;  //the first thing in the archive is the name of the protocol analyzer that the data belongs to.
    text_archive >> &name_string;

    if (strcmp(name_string, "QspiAnalyzer") != 0) {
        AnalyzerHelpers::Assert("QspiAnalyzer: Provided with a settings string that doesn't belong to us;");
    }

    text_archive >> mData0Channel;
    text_archive >> mData1Channel;
    text_archive >> mData2Channel;
    text_archive >> mData3Channel;
    text_archive >> mSckChannel;
    text_archive >> mCsChannel;

    text_archive >>  *(U32 *)&mShiftOrder;
    text_archive >> *(U32*)&mClockIdle;
    text_archive >> *(U32*)&mDataValidEdge;
    text_archive >>  *(U32 *)&mEnableActiveState;

    text_archive >> mCommandBits;
    text_archive >> mAddressBits;
    text_archive >> mDummyBits;
    text_archive >> mDataBits;

    text_archive >> *(U32*)&mCommandIOMode;
    text_archive >> *(U32*)&mAddressIOMode;
    text_archive >> *(U32*)&mDataIOMode;
    text_archive >> *(U32*)&mDataRate;

    text_archive >> *(U32*)&mCsPreCycles;
    text_archive >> *(U32*)&mCsPostCycles;

    text_archive >> mShowMarker;

    bool show_marker;
    if (text_archive >> show_marker) {
        mShowMarker = show_marker;
    }

    ClearChannels();

    AddChannel(mData0Channel, "DQ0", mData0Channel != UNDEFINED_CHANNEL);
    AddChannel(mData1Channel, "DQ1", mData1Channel != UNDEFINED_CHANNEL);
    AddChannel(mData2Channel, "DQ2", mData2Channel != UNDEFINED_CHANNEL);
    AddChannel(mData3Channel, "DQ3", mData3Channel != UNDEFINED_CHANNEL);
    AddChannel(mSckChannel, "SCK", mSckChannel != UNDEFINED_CHANNEL);
    AddChannel(mCsChannel, "CS", mCsChannel != UNDEFINED_CHANNEL);

    UpdateInterfacesFromSettings();
}

const char *QSpiAnalyzerSettings::SaveSettings()
{
    SimpleArchive text_archive;

    text_archive << "QspiAnalyzer";
    //text_archive <<  mClockInactiveState;

    text_archive << mData0Channel;
    text_archive << mData1Channel;
    text_archive << mData2Channel;
    text_archive << mData3Channel;
    text_archive << mSckChannel;
    text_archive << mCsChannel;

    text_archive << mShiftOrder;
    text_archive << mClockIdle;
    text_archive << mDataValidEdge;
    text_archive << mEnableActiveState;

    text_archive << mCommandBits;
    text_archive << mAddressBits;
    text_archive << mDummyBits;
    text_archive << mDataBits;

    text_archive << mCommandIOMode;
    text_archive << mAddressIOMode;
    text_archive << mDataIOMode;
    text_archive << mDataRate;

    text_archive << mCsPreCycles;
    text_archive << mCsPostCycles;

    text_archive << mShowMarker;


    return SetReturnString(text_archive.GetString());
}

void QSpiAnalyzerSettings::UpdateInterfacesFromSettings()
{

    mData0ChannelInterface->SetChannel(mData0Channel);
    mData1ChannelInterface->SetChannel(mData1Channel);
    mData2ChannelInterface->SetChannel(mData2Channel);
    mData3ChannelInterface->SetChannel(mData3Channel);
    mSckChannelInterface->SetChannel(mSckChannel);
    mCsChannelInterface->SetChannel(mCsChannel);

    mShiftOrderInterface->SetNumber(mShiftOrder);
    mClockIdleInterface->SetNumber(mClockIdle);
    mDataValidEdgeInterface->SetNumber(mDataValidEdge);
    mEnableActiveStateInterface->SetNumber(mEnableActiveState);

    mCommandBitsInterface->SetNumber(mCommandBits);
    mAddressBitsInterface->SetNumber(mAddressBits);
    mDummyBitsInterface->SetNumber(mDummyBits);
    mDataBitsInterface->SetNumber(mDataBits);

    mCommandIOModeInterface->SetNumber(mCommandIOMode);
    mAddressIOModeInterface->SetNumber(mAddressIOMode);
    mDataIOModeInterface->SetNumber(mDataIOMode);

    mDataRateInterface->SetNumber(mDataRate);
    mCsPreEnabledInterface->SetNumber(mCsPreCycles);
    mCsPostEnabledInterface->SetNumber(mCsPostCycles);

    mUseShowMarkerInterface->SetValue(mShowMarker);
}