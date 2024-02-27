#ifndef QSPI_ANALYZER_H
#define QSPI_ANALYZER_H

#if SOFTWARE == SALEAE
#define ANALYZER_VERSION Analyzer2
#elif SOFTWARE == KINGSTVIS
#define ANALYZER_VERSION Analyzer
#else
#error Not supported or undefined
#endif

#include <Analyzer.h>
#include "QSpiAnalyzerResults.h"
#include "QSpiSimulationDataGenerator.h"
#include "QspiTypes.h"

class QSpiAnalyzerSettings;

class ANALYZER_EXPORT QSpiAnalyzer : public ANALYZER_VERSION
{
public:
    QSpiAnalyzer();
    virtual ~QSpiAnalyzer();
    virtual void SetupResults();
    virtual void WorkerThread();

    virtual U32 GenerateSimulationData(U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor **simulation_channels);
    virtual U32 GetMinimumSampleRateHz();

    virtual const char *GetAnalyzerName() const;
    virtual bool NeedsRerun();

protected: //functions
    void Setup();
    void AdvanceToActiveEnableEdge();
    bool IsInitialClockPolarityCorrect();
    void AdvanceToActiveEnableEdgeWithCorrectClockPolarity();
    bool WouldAdvancingTheClockToggleEnable();

    void GetBlock();    
    void AdvanceState();
    void GetDummyBlock();
    void MarkChannels(const U64 sampleNum, const AnalyzerResults::MarkerType marker, const U32 channelMask);

#pragma warning( push )
#pragma warning( disable : 4251 ) //warning C4251: 'SerialAnalyzer::<...>' : class <...> needs to have dll-interface to be used by clients of class
protected:  //vars

    const U8 mStartStopMask = 0b100000;

    std::unique_ptr<QSpiAnalyzerSettings> mSettings;
    std::unique_ptr<QSpiAnalyzerResults> mResults;
    bool mSimulationInitilized;
    QSpiSimulationDataGenerator mSimulationDataGenerator;
    QSpiDataChannelManager<AnalyzerChannelData> mQSpiChannelManager;

    QSpiTypes::TransactionState mTransactionState;

    /*AnalyzerChannelData** mChannels[6] = {
        &mData0,
        &mData1,
        &mData2,
        &mData3,
        &mSck,
        &mCs
    };*/

    AnalyzerChannelData* mData0;
    AnalyzerChannelData* mData1;
    AnalyzerChannelData* mData2;
    AnalyzerChannelData* mData3;

    AnalyzerChannelData* mSck;
    AnalyzerChannelData* mCs;

    U64 mCurrentSample;
    AnalyzerResults::MarkerType mArrowMarker;
    std::vector<U64> mArrowLocations;

#pragma warning( pop )
};

extern "C" ANALYZER_EXPORT const char *__cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer *__cdecl CreateAnalyzer();
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer(Analyzer *analyzer);

#endif //QSPI_ANALYZER_H
