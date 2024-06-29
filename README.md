# DHT11
DHT11 library to communicate with DHT11 sensor

## Key features
- Compatible with `Arduino IDE` & `Microchip Studio IDE`
- Easy to use functions such as ```begin()```, ```getTemperature()``` & ```getHumidity()```
- Non-blocking as it uses time keeping logic and a state machine switch mechanism with minimal overhead of wasted time
- Limited to new values received for only every 2 seconds as DHT11 is very slow

## Tested on
- ATmega328P @16MHz with `Arduino IDE` & `Microchip Studio IDE`
