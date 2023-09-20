#ifndef QSPI_SIMULATION_DATA_GENERATOR
#define QSPI_SIMULATION_DATA_GENERATOR

#include <AnalyzerHelpers.h>
#include "QSpiDataChannelManager.h"

class QSpiAnalyzerSettings;

class QSpiSimulationDataGenerator
{
public:
    QSpiSimulationDataGenerator();
    ~QSpiSimulationDataGenerator();

    void Initialize(U32 simulation_sample_rate, QSpiAnalyzerSettings *settings);
    U32 GenerateSimulationData(U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor **simulation_channels);

protected:
    QSpiAnalyzerSettings *mSettings;
    U32 mSimulationSampleRateHz;
    U64 mValue;

protected: //QSPI specific
    ClockGenerator mClockGenerator;
    SimulationChannelDescriptorGroup mQSpiSimulationChannels;
    QSpiDataChannelManager<SimulationChannelDescriptor> mQSpiChannelManager;
    
    void CreateQSpiTransaction();
    void OutputBits(const U64 data, const U32 bitlen, const QSpiTypes::IOMode ioMode, const AnalyzerEnums::Edge validEdge);
    void OutputBits_CPHA0(const U64 data, const U32 bitlen, const QSpiTypes::IOMode ioMode);
    void OutputBits_CPHA1(const U64 data, const U32 bitlen, const QSpiTypes::IOMode ioMode);

    SimulationChannelDescriptor* mData0;
    SimulationChannelDescriptor* mData1;
    SimulationChannelDescriptor* mData2;
    SimulationChannelDescriptor* mData3;
    SimulationChannelDescriptor* mSck;
    SimulationChannelDescriptor* mCs;
};
#endif //QSPI_SIMULATION_DATA_GENERATOR
