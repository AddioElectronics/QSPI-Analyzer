#pragma once

#include "QspiTypes.h"
#include "QSpiAnalyzerSettings.h"

static QSpiTypes::IOMode GetIOMode(const QSpiAnalyzerSettings* settings, const QSpiTypes::TransactionState state)
{
    switch (state)
    {
    case QSpiTypes::COMMAND_STATE:
        return settings->mCommandIOMode;
    case QSpiTypes::ADDRESS_STATE:
        return settings->mAddressIOMode;
    case QSpiTypes::DUMMY_STATE:
        return QSpiTypes::SIO;
    case QSpiTypes::DATA_STATE:
        return settings->mDataIOMode;
    default:
        return QSpiTypes::QIO;
    }
}

static QSpiTypes::TransactionState GetNextState(const QSpiAnalyzerSettings* settings, const QSpiTypes::TransactionState state)
{
    switch (state)
    {
    case QSpiTypes::SETUP_STATE:
    default:
        if (settings->mCommandBits > 0) {
            return QSpiTypes::COMMAND_STATE;
        }
    case QSpiTypes::COMMAND_STATE:
        if (settings->mAddressBits > 0) {
            return QSpiTypes::ADDRESS_STATE;
        }
    case QSpiTypes::ADDRESS_STATE:
        if (settings->mDummyBits > 0) {
            return QSpiTypes::DUMMY_STATE;
        }
    case QSpiTypes::DUMMY_STATE:
        return QSpiTypes::DATA_STATE;
    case QSpiTypes::DATA_STATE:
        return QSpiTypes::SETUP_STATE;
    }
}

static U32 GetStateBitCount(const QSpiAnalyzerSettings* settings, const QSpiTypes::TransactionState state)
{
    switch (state)
    {
    case QSpiTypes::COMMAND_STATE:
        return settings->mCommandBits;
    case QSpiTypes::ADDRESS_STATE:
        return settings->mAddressBits;
    case QSpiTypes::DUMMY_STATE:
        return settings->mDummyBits;
    case QSpiTypes::DATA_STATE:
        return settings->mDataBits;
    default:
        return 8;
    }

}

static U32 GetStateClockCount(const QSpiAnalyzerSettings* settings, QSpiTypes::TransactionState state, const  QSpiTypes::IOMode ioMode)
{
    U32 bits = GetStateBitCount(settings, state);
    return bits / (U32)ioMode;
}

static U8 GetChannelMask(const QSpiAnalyzerSettings* settings, const QSpiTypes::TransactionState state)
{
    QSpiTypes::IOMode dataLines = GetIOMode(settings, state);

    switch (dataLines)
    {
    case QSpiTypes::SIO:
        return 0b0001;
    case QSpiTypes::DIO:
        return 0b0011;
    case QSpiTypes::QIO:
        return 0b1111;
    default:
        return 0;
    }
}

static U32 GetDataChannelBitCount(const U32 channel, const U32 bitCount, const QSpiTypes::IOMode ioMode)
{
    if (channel + 1 > (U32)ioMode)
        return 0;

    U32 bits_per_channel = bitCount / (U32)ioMode;
    return bits_per_channel;
}


