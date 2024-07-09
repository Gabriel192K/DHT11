#ifndef __DHT11_H__
#define __DHT11_H__

/* Dependencies */
#if defined(ARDUINO)
#include "Arduino.h"
#include <GPIO.h>
#include <Time.h>
#else
#include "../GPIO/GPIO.h"
#include "../Time/Time.h"
#endif

#if defined(ARDUINO)
#define beginTime()       0
#define getMicroseconds() micros()
#else // Microchip Studio
#define beginTime()       Time.begin()
#define getMicroseconds() Time.microseconds()
#endif

#define DHT11_WAIT_LOW_INTERVAL   (const uint32_t)18000   // in microseconds
#define DHT11_WAIT_20_US_INTERVAL (const uint32_t)20      // in microseconds
#define DHT11_WAIT_30_US_INTERVAL (const uint32_t)30      // in microseconds
#define DHT11_WAIT_NEXT_PACKET_US (const uint32_t)2000000 // in microseconds

enum DHT11_State
{
    DHT11_REQUEST_LOW,
    DHT11_REQUEST_WAIT_LOW,
    DHT11_REQUEST_WAIT_20_US,
    DHT11_RESPONSE_WAIT_START,
    DHT11_RESPONSE_WAIT_HIGH,
    DHT11_RESPONSE_WAIT_LOW,
    DHT11_DATA_READ_START,
    DHT11_DATA_WAIT_HIGH,
    DHT11_DATA_WAIT_30_US,
    DHT11_DATA_WAIT_LOW,
    DHT11_WAIT_NEXT_PACKET
};

class DHT11
{
    public:
        DHT11(GPIO* dhtGPIO);
        ~DHT11();
        void        begin         (void);
        void        run           (void);
        const float getTemperature(void);
        const float getHumidity   (void);

    private:
        GPIO* dhtGPIO;
        uint32_t nextTime;
        float humidity;
        float temperature;
        uint8_t state;
        uint8_t data;
        uint8_t bitIndex;
        uint8_t byteIndex;
        uint8_t receivedData[5]; // 5 bytes: I_RH, D_RH, I_Temp, D_Temp, Checksum

        void requestLow();
        void waitLow();
        void wait20us();
        void waitStartResponse();
        void waitResponseHigh();
        void waitResponseLow();
        void startDataRead();
        void waitDataHigh();
        void waitData30us();
        void waitDataLow();
        void waitNextPacket();
};

#endif // DHT11_H
