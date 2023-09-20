#pragma once

namespace QSpiTypes
{
    enum CPOL {
        CPOL0 = 0,
        CPOL1 = 1,
    };

    enum CPHA {
        CPHA0 = 0,
        CPHA1 = 1,
    };

    enum DataRate {
        SDR = 0,
        DDR = 1,
    };

    enum IOMode {
        SIO = 1,
        DIO = 2,
        QIO = 4,
        //OIO = 8
    };

    enum TransactionState {
        SETUP_STATE,
        COMMAND_STATE,
        ADDRESS_STATE,
        DUMMY_STATE,
        DATA_STATE
    };
};