/* Dependencies */
#include <DHT11.h>
#include <GPIO.h>

/* Macros */
#define DHT11_DDR DDRD
#define DHT11_DOR PORTD
#define DHT11_DIR PIND
#define DHT11_BIT (const uint8_t)2

/* Objects */
GPIO dhtGPIO(&DHT11_DDR, &DHT11_DOR, &DHT11_DIR, DHT11_BIT);
DHT11 dht11(&dhtGPIO);

void setup(void)
{
    Serial.begin(115200);
    dht11.begin();
}

void loop(void)
{
    dht11.run();
    static uint32_t nextTime = 1000;
    if (millis() < nextTime)
        return;

    const float temperature = dht11.getTemperature();
    const float humidity = dht11.getHumidity();
    Serial.print(F("Temperature: "));
    Serial.println(temperature);
    Serial.print(F("Humidity: "));
    Serial.println(humidity);
    nextTime = millis() + 1000;
}
