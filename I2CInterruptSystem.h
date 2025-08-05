/// @copyright Copyright © 2025 ygdstmidn
/// @license This file is released under the MIT License(https://opensource.org/license/mit)
#ifndef __INCLUDE_I2C_INTERRUPT_SYSTEM_H
#define __INCLUDE_I2C_INTERRUPT_SYSTEM_H

#include <main.h>
#include <vector>

namespace I2CInterruptSystem
{

    enum I2CInterruptSystemType
    {
        MasterTransmit,
        MasterReceive,
        MemWrite,
        MemRead
    };

    struct I2CInterruptSystemData
    {
        const I2CInterruptSystemType Type;
        const uint16_t DevAddress;
        uint8_t *pData;
        const uint16_t Size;
        const uint16_t MemAddress;
        const uint16_t MemAddSize;
        int sendFlag = 0;
    };

    class I2CInterruptSystem
    {
    private:
        I2C_HandleTypeDef *_hi2c;
        std::vector<I2CInterruptSystemData> _DataQueue; // push_backが使いたかった
        unsigned int _DataQueueIndex = 0;
        int _startFlag = 0; // 0:停止, 1:開始
        int _state = 0;// 0:停止, 1:次はsend, 2:次はリセット
        void send(void);
        void reset(void);

    public:
        I2CInterruptSystem(I2C_HandleTypeDef *hi2c);
        ~I2CInterruptSystem(void);

        void start(void);
        void stop(void);
        void loop(void);
        int sendCheck(const unsigned int index);
        unsigned int addMasterTransmit(const uint16_t DevAddress, uint8_t *pData, const uint16_t Size);
        unsigned int addMasterReceive(const uint16_t DevAddress, uint8_t *pData, const uint16_t Size);
        unsigned int addMemWrite(const uint16_t DevAddress, const uint16_t MemAddress, const uint16_t MemAddSize, uint8_t *pData, const uint16_t Size);
        unsigned int addMemRead(const uint16_t DevAddress, const uint16_t MemAddress, const uint16_t MemAddSize, uint8_t *pData, const uint16_t Size);
        void MasterTxCpltCallback(const I2C_HandleTypeDef *hi2c);
        void MasterRxCpltCallback(const I2C_HandleTypeDef *hi2c);
        void MemTxCpltCallback(const I2C_HandleTypeDef *hi2c);
        void MemRxCpltCallback(const I2C_HandleTypeDef *hi2c);
        void ErrorCallback(const I2C_HandleTypeDef *hi2c);
    };
} // namespace I2CInterruptSystem

#endif // __INCLUDE_I2C_INTERRUPT_SYSTEM_H
