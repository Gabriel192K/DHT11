#include "DHT11.h"

/*!
 * @brief  DHT11 constructor
 * @param  dhtGPIO
 *         The GPIO of DHT11
 */
DHT11::DHT11(GPIO* dhtGPIO)
{
    this->dhtGPIO = dhtGPIO;
}

/*!
 * @brief  DHT11 destructor
 */
DHT11::~DHT11()
{
    this->dhtGPIO = NULL;
}

/*!
 * @brief  Begins the DHT11 implementation
 */
void DHT11::begin(void)
{
    beginTime();
    this->state = REQUEST_LOW;
}

/*!
 * @brief  DHT11 main loop
 */
void DHT11::run(void)
{
    switch (this->state)
    {
        case REQUEST_LOW:
            requestLow();
            break;
        case REQUEST_WAIT_LOW:
            waitLow();
            break;
        case REQUEST_WAIT_20_US:
            wait20us();
            break;
        case RESPONSE_WAIT_START:
            waitStartResponse();
            break;
        case RESPONSE_WAIT_HIGH:
            waitResponseHigh();
            break;
        case RESPONSE_WAIT_LOW:
            waitResponseLow();
            break;
        case DATA_READ_START:
            startDataRead();
            break;
        case DATA_WAIT_HIGH:
            waitDataHigh();
            break;
        case DATA_WAIT_30_US:
            waitData30us();
            break;
        case DATA_WAIT_LOW:
            waitDataLow();
            break;
        case WAIT_NEXT_PACKET:
            waitNextPacket();
            break;
    }
}

/*!
 * @brief  Getting the value of temperature
 * @return Value of temperature as a floating point number
 */
const float DHT11::getTemperature(void)
{
    return this->temperature;
}

/*!
 * @brief  Getting the value of humidity
 * @return Value of humidity as a floating point number
 */
const float DHT11::getHumidity(void) 
{
    return this->humidity;
}

void DHT11::requestLow(void)
{
    this->dhtGPIO->setMode(OUTPUT);
    this->dhtGPIO->write(LOW);
    this->nextTime = getMicroseconds() + WAIT_LOW_INTERVAL;
    this->state = REQUEST_WAIT_LOW;
}

void DHT11::waitLow(void)
{
    if (getMicroseconds() < this->nextTime)
        return;
    
    this->dhtGPIO->write(HIGH);
    this->nextTime = getMicroseconds() + WAIT_20_US_INTERVAL;
    this->state = REQUEST_WAIT_20_US;
}

void DHT11::wait20us(void)
{
    if (getMicroseconds() < this->nextTime)
        return;

    this->dhtGPIO->setMode(INPUT);
    this->state = RESPONSE_WAIT_START;
}

void DHT11::waitStartResponse(void)
{
    if (this->dhtGPIO->read())
        return;
    
    this->state = RESPONSE_WAIT_HIGH;
}

void DHT11::waitResponseHigh(void)
{
    if (!this->dhtGPIO->read())
        return;
    
    this->state = RESPONSE_WAIT_LOW;
}

void DHT11::waitResponseLow(void)
{
    if (this->dhtGPIO->read())
        return;
    
    this->state = DATA_READ_START;
}

void DHT11::startDataRead(void)
{
    this->data = 0;
    this->bitIndex = 0;
    this->byteIndex = 0;
    this->state = DATA_WAIT_HIGH;
}

void DHT11::waitDataHigh(void)
{
    if (!this->dhtGPIO->read())
        return;
    
    this->nextTime = getMicroseconds() + WAIT_30_US_INTERVAL;
    this->state = DATA_WAIT_30_US;
}

void DHT11::waitData30us(void)
{
    if (getMicroseconds() < this->nextTime)
        return;
    
    // if pin is still high after 30us, it's a '1' else its a '0'
    if (this->dhtGPIO->read()) 
        this->data = (this->data << 1) | 1;
    else
        this->data = (this->data << 1);

    this->state = DATA_WAIT_LOW;
}

void DHT11::waitDataLow(void)
{
    if (this->dhtGPIO->read())
        return;

    this->bitIndex++;
    if (this->bitIndex >= 8)
    {
        this->receivedData[this->byteIndex++] = this->data;
        if (this->byteIndex >= 5)
        {
            // All 5 bytes received
            const uint8_t I_RH = this->receivedData[0];     // integer RH
            const uint8_t D_RH = this->receivedData[1];     // decimal RH
            const uint8_t I_Temp = this->receivedData[2];   // integer Temp
            const uint8_t D_Temp = this->receivedData[3];   // decimal Temp
            const uint8_t checksum = this->receivedData[4]; // checksum

            if ((I_RH + D_RH + I_Temp + D_Temp) == checksum)
            {
                this->humidity = (float)I_RH + (float)D_RH / 10;
                this->temperature = (float)I_Temp + (float)D_Temp / 10;
            }
            this->nextTime = getMicroseconds() + WAIT_NEXT_PACKET_US;
            this->state = WAIT_NEXT_PACKET;
            return;
        }
        this->data = 0;
        this->bitIndex = 0;
    }

    this->state = DATA_WAIT_HIGH;
}

void DHT11::waitNextPacket(void)
{
    if (getMicroseconds() < this->nextTime)
        return;

    this->state = REQUEST_LOW;
}
