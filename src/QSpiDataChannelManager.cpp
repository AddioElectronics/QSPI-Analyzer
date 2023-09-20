
#ifndef QSPI_DATA_CHANNEL_MANAGER_CPP
#define QSPI_DATA_CHANNEL_MANAGER_CPP

#ifndef QSPI_DATA_CHANNEL_MANAGER_H
#include "QSpiDataChannelManager.h"
#endif

template <typename T>
void QSpiDataChannelManager<T>::Init(T* dataChannel0, T* dataChannel1, T* dataChannel2, T* dataChannel3, AnalyzerEnums::ShiftOrder endianess, QSpiTypes::IOMode ioMode)
{
    _endianess = endianess;
    _ioMode = ioMode;
    _channels[0] = dataChannel0;
    _channels[1] = dataChannel1;
    _channels[2] = dataChannel2;
    _channels[3] = dataChannel3;
}


template <typename T>
T* QSpiDataChannelManager<T>::NextChannel()
{
    T* channel = _channels[_index];

    if (_endianess == AnalyzerEnums::LsbFirst){
        _index++;
    }
    else{
        _index--;
    }

    if (_index > _maxIndex)
        _index = _startIndex;

    return channel;
}

template <typename T>
T* QSpiDataChannelManager<T>::CurrentChannel()
{
    return _channels[_index];
}

template <typename T>
void QSpiDataChannelManager<T>::Reset()
{
    if (_endianess == AnalyzerEnums::LsbFirst){
        _index = 0;
    }
    else{
        _index = _ioMode - 1;
    }

    _startIndex = _index;
    _maxIndex = _ioMode - 1;
}

template <typename T>
U8 QSpiDataChannelManager<T>::GetIndex()
{
    return _index;
}

template <typename T>
void QSpiDataChannelManager<T>::SetIndex(U8 index)
{
    if (index > _maxIndex)
        return;

    _index = index;
}

template <typename T>
void QSpiDataChannelManager<T>::SetIOMode(QSpiTypes::IOMode ioMode)
{
    _ioMode = ioMode;
    Reset();
}

template <typename T>
QSpiTypes::IOMode QSpiDataChannelManager<T>::GetIOMode()
{
    return _ioMode;
}

template <typename T>
void QSpiDataChannelManager<T>::SetEndianess(AnalyzerEnums::ShiftOrder endianess)
{
    _endianess = endianess;
    Reset();
}

template <typename T>
AnalyzerEnums::ShiftOrder QSpiDataChannelManager<T>::GetEndianess()
{
    return _endianess;
}

#endif //QSPI_DATA_CHANNEL_MANAGER_CPP

