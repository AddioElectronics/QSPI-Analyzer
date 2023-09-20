#ifndef SPI_ANALYZER_SETTINGS
#define SPI_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

#include "QspiTypes.h"

class QSpiAnalyzerSettings : public AnalyzerSettings
{
public:
    QSpiAnalyzerSettings();
    virtual ~QSpiAnalyzerSettings();

    virtual bool SetSettingsFromInterfaces();
    virtual void LoadSettings(const char *settings);
    virtual const char *SaveSettings();

    void UpdateInterfacesFromSettings();

    //BitState mClockInactiveState;


    Channel* mDataChannels[4] = {
        &mData0Channel,
        & mData1Channel,
        & mData2Channel,
        & mData3Channel
    };

    Channel* mChannels[6] = {
        &mData0Channel,
        &mData1Channel,
        &mData2Channel,
        &mData3Channel,
        &mSckChannel,
        &mCsChannel
    };

    Channel mData0Channel;
    Channel mData1Channel;
    Channel mData2Channel;
    Channel mData3Channel;
    Channel mSckChannel;
    Channel mCsChannel;

    AnalyzerEnums::ShiftOrder mShiftOrder;
    BitState mClockIdle;
    AnalyzerEnums::Edge mDataValidEdge;
    BitState mEnableActiveState;

    U32 mCommandBits;
    U32 mAddressBits;
    U32 mDummyBits;
    U32 mDataBits;

    QSpiTypes::IOMode mCommandIOMode;
    QSpiTypes::IOMode mAddressIOMode;
    QSpiTypes::IOMode mDataIOMode;
    QSpiTypes::DataRate mDataRate;

    U32 mCsPreCycles;
    U32 mCsPostCycles;
    bool  mShowMarker;

protected:

	std::unique_ptr< AnalyzerSettingInterfaceChannel > mData0ChannelInterface;
	std::unique_ptr< AnalyzerSettingInterfaceChannel > mData1ChannelInterface;
	std::unique_ptr< AnalyzerSettingInterfaceChannel > mData2ChannelInterface;
	std::unique_ptr< AnalyzerSettingInterfaceChannel > mData3ChannelInterface;
	std::unique_ptr< AnalyzerSettingInterfaceChannel > mSckChannelInterface;
	std::unique_ptr< AnalyzerSettingInterfaceChannel > mCsChannelInterface;

	std::unique_ptr< AnalyzerSettingInterfaceNumberList > mShiftOrderInterface;
	std::unique_ptr< AnalyzerSettingInterfaceNumberList > mClockIdleInterface;
	std::unique_ptr< AnalyzerSettingInterfaceNumberList > mDataValidEdgeInterface;
	std::unique_ptr< AnalyzerSettingInterfaceNumberList > mEnableActiveStateInterface;

	std::unique_ptr< AnalyzerSettingInterfaceNumberList > mCommandBitsInterface;
	std::unique_ptr< AnalyzerSettingInterfaceNumberList > mAddressBitsInterface;
	std::unique_ptr< AnalyzerSettingInterfaceNumberList > mDummyBitsInterface;
    std::unique_ptr< AnalyzerSettingInterfaceNumberList > mDataBitsInterface;

	std::unique_ptr< AnalyzerSettingInterfaceNumberList > mCommandIOModeInterface;
	std::unique_ptr< AnalyzerSettingInterfaceNumberList > mAddressIOModeInterface;
    std::unique_ptr< AnalyzerSettingInterfaceNumberList > mDataIOModeInterface;
	std::unique_ptr< AnalyzerSettingInterfaceNumberList > mDataRateInterface;

	std::unique_ptr< AnalyzerSettingInterfaceNumberList > mCsPreEnabledInterface;
	std::unique_ptr< AnalyzerSettingInterfaceNumberList > mCsPostEnabledInterface;


    std::unique_ptr< AnalyzerSettingInterfaceBool > mUseShowMarkerInterface;
};

#endif //SPI_ANALYZER_SETTINGS
