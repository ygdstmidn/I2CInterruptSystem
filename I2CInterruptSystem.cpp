/// @copyright Copyright © 2025 ygdstmidn
/// @license This file is released under the MIT License(https://opensource.org/license/mit)

#include "I2CInterruptSystem.h"

namespace I2CInterruptSystem
{
    I2CInterruptSystem::I2CInterruptSystem(I2C_HandleTypeDef *hi2c)
    {
        _hi2c = hi2c;
    }

    I2CInterruptSystem::~I2CInterruptSystem(void)
    {
    }

    void I2CInterruptSystem::send(void)
    {
        if (_DataQueueIndex >= _DataQueue.size())
        {
            _DataQueueIndex = 0;
        }

        if (__HAL_I2C_GET_FLAG(_hi2c, I2C_FLAG_BUSY))
        {//error 多分断線
            _state = 2;//もう一度リセット
            return;
        }

        I2CInterruptSystemData &data = _DataQueue[_DataQueueIndex];
        HAL_StatusTypeDef status;
        switch (data.Type)
        {
            case MasterTransmit:
                status = HAL_I2C_Master_Transmit_IT(_hi2c, data.DevAddress, data.pData, data.Size);
                break;

            case MasterReceive:
                status = HAL_I2C_Master_Receive_IT(_hi2c, data.DevAddress, data.pData, data.Size);
                break;

            case MemWrite:
                status = HAL_I2C_Mem_Write_IT(_hi2c, data.DevAddress, data.MemAddress, data.MemAddSize, data.pData, data.Size);
                break;

            case MemRead:
                status = HAL_I2C_Mem_Read_IT(_hi2c, data.DevAddress, data.MemAddress, data.MemAddSize, data.pData, data.Size);
                break;

            default://ありえないはず
                break;
        }

        if (status == HAL_OK)
        {
            data.sendFlag++;
            _DataQueueIndex++;
        }
        else
        {
            _state = 2; // エラーが発生したので次はリセット
        }
    }

    void I2CInterruptSystem::reset(void)
    {
        HAL_I2C_DeInit(_hi2c);
        HAL_I2C_Init(_hi2c);
    }

    void I2CInterruptSystem::start(void)
    {
        _startFlag = 1; // 開始フラグ
        _state = 1;
    }

    /// @note 現在送信中のデータはキャンセルされません
    void I2CInterruptSystem::stop(void)
    {
        _startFlag = 0; // 停止フラグ
    }

    void I2CInterruptSystem::loop(void)
    {
        if (_startFlag && _state == 1)
        {
            _state = 0;
            send();
        }
        else if (_startFlag && _state == 2)
        {
            _state = 1; // 次はsend
            reset();
        }
    }

    int I2CInterruptSystem::sendCheck(const unsigned int index)
    {
        if (index < _DataQueue.size())
        {
            const int sendFlag = _DataQueue[index].sendFlag;
            _DataQueue[index].sendFlag = 0; // 送信フラグをリセット
            return sendFlag;
        }
        return -1; // 無効なインデックス
    }

    unsigned int I2CInterruptSystem::addMasterTransmit(const uint16_t DevAddress, uint8_t *pData, const uint16_t Size)
    {
        _DataQueue.push_back({MasterTransmit, DevAddress, pData, Size, 0, 0});
        return _DataQueue.size() - 1;
    }

    unsigned int I2CInterruptSystem::addMasterReceive(const uint16_t DevAddress, uint8_t *pData, const uint16_t Size)
    {
        _DataQueue.push_back({MasterReceive, DevAddress, pData, Size, 0, 0});
        return _DataQueue.size() - 1;
    }

    unsigned int I2CInterruptSystem::addMemWrite(const uint16_t DevAddress, const uint16_t MemAddress, const uint16_t MemAddSize, uint8_t *pData, const uint16_t Size)
    {
        _DataQueue.push_back({MemWrite, DevAddress, pData, Size, MemAddress, MemAddSize});
        return _DataQueue.size() - 1;
    }

    unsigned int I2CInterruptSystem::addMemRead(const uint16_t DevAddress, const uint16_t MemAddress, const uint16_t MemAddSize, uint8_t *pData, const uint16_t Size)
    {
        _DataQueue.push_back({MemRead, DevAddress, pData, Size, MemAddress, MemAddSize});
        return _DataQueue.size() - 1;
    }

    void I2CInterruptSystem::MasterTxCpltCallback(const I2C_HandleTypeDef *hi2c)
    {
        if (hi2c == _hi2c)
        {
            if (_startFlag)
            {
                _state = 1; // 次はsend
            }
        }
    }

    void I2CInterruptSystem::MasterRxCpltCallback(const I2C_HandleTypeDef *hi2c)
    {
        if (hi2c == _hi2c)
        {
            if (_startFlag)
            {
                _state = 1; // 次はsend
            }
        }
    }

    void I2CInterruptSystem::MemTxCpltCallback(const I2C_HandleTypeDef *hi2c)
    {
        if (hi2c == _hi2c)
        {
            if (_startFlag)
            {
                _state = 1; // 次はsend
            }
        }
    }

    void I2CInterruptSystem::MemRxCpltCallback(const I2C_HandleTypeDef *hi2c)
    {
        if (hi2c == _hi2c)
        {
            if (_startFlag)
            {
                _state = 1; // 次はsend
            }
        }
    }

    void I2CInterruptSystem::ErrorCallback(const I2C_HandleTypeDef *hi2c)
    {
        if (hi2c == _hi2c)
        {
            _state = 2; // 次はリセット
        }
    }
} // namespace I2CInterruptSystem
