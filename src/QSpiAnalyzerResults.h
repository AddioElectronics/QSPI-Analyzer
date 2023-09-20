#ifndef QSPI_ANALYZER_RESULTS
#define QSPI_ANALYZER_RESULTS

#include <AnalyzerResults.h>

#define QSPI_ERROR_FLAG ( 1 << 0 )

class QSpiAnalyzer;
class QSpiAnalyzerSettings;

class QSpiAnalyzerResults : public AnalyzerResults
{
public:
    QSpiAnalyzerResults(QSpiAnalyzer *analyzer, QSpiAnalyzerSettings *settings);
    virtual ~QSpiAnalyzerResults();

    virtual void GenerateBubbleText(U64 frame_index, Channel &channel, DisplayBase display_base);
    virtual void GenerateExportFile(const char *file, DisplayBase display_base, U32 export_type_user_id);

    virtual void GenerateFrameTabularText(U64 frame_index, DisplayBase display_base);
    virtual void GeneratePacketTabularText(U64 packet_id, DisplayBase display_base);
    virtual void GenerateTransactionTabularText(U64 transaction_id, DisplayBase display_base);

protected: //functions

protected: //vars
    QSpiAnalyzerSettings *mSettings;
    QSpiAnalyzer *mAnalyzer;

    U32 data_index = 0;
};

#endif //QSPI_ANALYZER_RESULTS
