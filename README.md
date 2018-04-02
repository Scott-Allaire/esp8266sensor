# Environment monitoring with ESP8266 and SmartThings v2

## Setting up the ESP8266

This project uses the [Makerfocus ESP8266 Module ESP-12E](https://smile.amazon.com/gp/product/B01IK9GEQG/ref=oh_aui_detailpage_o00_s00?ie=UTF8&psc=1), a DHT11 sensor and a photoresistor.  The ESP8266 has the NodeMCU firmware installed. The Lua script reads information from the sensors and answers HTTP requests on port 80.

The SmartThings device handler allows the creation of a device that can be configured with IP address, port and MAC address of the sensor.

## Setting up the Device Handler and Device on SmartThings

Coming soon...

## References

Thanks to [stjohnjohnson](https://gist.github.com/stjohnjohnson/5dfe4d5caf3e2afa5a56) and related discussion in the [SmartThings Community](https://community.smartthings.com/t/how-to-send-data-from-my-lan-connected-device-to-hubv2/28825) for some great information about SmartThings device handlers.