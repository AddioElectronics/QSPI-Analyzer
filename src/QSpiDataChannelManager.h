#ifndef QSPI_DATA_CHANNEL_MANAGER_H
#define QSPI_DATA_CHANNEL_MANAGER_H

#include "AnalyzerTypes.h"
#include "QspiTypes.h"

template <typename T>
class QSpiDataChannelManager
{
public:
	 void Init(T* dataChannel0, T* dataChannel1, T* dataChannel2, T* dataChannel3, AnalyzerEnums::ShiftOrder endianess = AnalyzerEnums::MsbFirst, QSpiTypes::IOMode ioMode = QSpiTypes::QIO);

	 T* NextChannel();
	 T* CurrentChannel();
	 void Reset();
	 U8 GetIndex();
	 void SetIndex(U8 index);

	 void SetIOMode(QSpiTypes::IOMode ioMode = QSpiTypes::QIO);
	 QSpiTypes::IOMode GetIOMode();

	 void SetEndianess(AnalyzerEnums::ShiftOrder endianess);
	 AnalyzerEnums::ShiftOrder GetEndianess();


private:
	T* _channels[4];
	U8 _index;
	U8 _startIndex;
	U8 _maxIndex;
	QSpiTypes::IOMode _ioMode = QSpiTypes::QIO;
	AnalyzerEnums::ShiftOrder _endianess;
};

#endif //QSPI_DATA_CHANNEL_MANAGER_H

#include "QSpiDataChannelManager.cpp"
