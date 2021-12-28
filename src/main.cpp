// matth-x/ArduinoOcpp
// Copyright Matthias Akstaller 2019 - 2021
// MIT License

#include <Arduino.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti WiFiMulti;
#elif defined(ESP32)
#include <WiFi.h>
#else
#error only ESP32 or ESP8266 supported at the moment
#endif

#include <ArduinoOcpp.h>

// #define STASSID "jjun"
// #define STAPSK  "26683874"

// #define STASSID "ChargEV5G\0"
// #define STAPSK  "Ch@rg2V5G\0"

#define STASSID "ChargEV2G\0"
#define STAPSK  "Ch@rg2V2G\0"

//#define OCPP_HOST "192.168.0.7\0"
#define OCPP_HOST "192.168.0.212\0"
#define OCPP_PORT 18050
//#define OCPP_URL "ws://192.168.0.7:18050\0"
#define OCPP_URL "/TT00000011"

//
////  Settings which worked for my SteVe instance
//
//#define OCPP_HOST "my.instance.com"
//#define OCPP_PORT 80
//#define OCPP_URL "ws://my.instance.com/steve/websocket/CentralSystemService/gpio-based-charger"

void setup() {

    /*
     * Initialize Serial and WiFi
     */ 

    Serial.begin(115200);
    Serial.setDebugOutput(true);

    Serial.print(F("[main] Wait for WiFi: "));

#if defined(ESP8266)
    WiFiMulti.addAP(STASSID, STAPSK);
    while (WiFiMulti.run() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);
    }
#elif defined(ESP32)
    WiFi.begin(STASSID, STAPSK);
    Serial.print(F("[main] Wait for WiFi: "));
    while (!WiFi.isConnected()) {
        Serial.print('.');
        delay(1000);
    }
#else
#error only ESP32 or ESP8266 supported at the moment
#endif

    Serial.print(F(" connected!\n"));

    /*
     * Initialize the OCPP library
     */
    OCPP_initialize(OCPP_HOST, OCPP_PORT, OCPP_URL);

    /*
     * Integrate OCPP functionality. You can leave out the following part if your EVSE doesn't need it.
     */
    setPowerActiveImportSampler([]() {
        //measure the input power of the EVSE here and return the value in Watts
        return 0.f;
    });

    setOnChargingRateLimitChange([](float limit) {
        //set the SAE J1772 Control Pilot value here
        Serial.print(F("[main] Smart Charging allows maximum charge rate: "));
        Serial.println(limit);
    });

    setEvRequestsEnergySampler([]() {
        //return true if the EV is in state "Ready for charging" (see https://en.wikipedia.org/wiki/SAE_J1772#Control_Pilot)
        return false;
    });

    //... see ArduinoOcpp.h for more settings

    /*
     * Notify the Central System that this station is ready
     */
    bootNotification("My Charging Station", "My company name"); //
}

void loop() {

    if(Serial.available()>0){
        char i = Serial.read();
        Serial.printf("Read : %c\r\n", i);
    }
    
    /*
     * Do all OCPP stuff (process WebSocket input, send recorded meter values to Central System, etc.)
     */
    OCPP_loop();

    /*
     * Get transaction state of OCPP
     */
    if (getTransactionId() > 0) {
        //transaction running with txID given by getTransactionId()
    } else if (getTransactionId() == 0) {
        //transaction initiation is pending, i.e. startTransaction() was already sent, but hasn't come back yet.
    } else {
        //no transaction running at the moment
    }

    /*
     * Detect if something physical happened at your EVSE and trigger the corresponding OCPP messages
     */
    if (/* RFID chip detected? */ false) {
        String idTag = "my-id-tag"; //e.g. idTag = RFID.readIdTag();
        authorize(idTag);
    }
    
    if (/* EV plugged in? */ false) {
        Serial.printf("111\r\n");
        startTransaction([] (JsonObject payload) {
            //Callback: Central System has answered. Energize your EV plug inside this callback and flash a confirmation light if you want.
            Serial.print(F("[main] Started OCPP transaction. EV plug energized\n"));
        });
    }
    
    if (/* EV unplugged? */ false) {
        Serial.printf("222\r\n");
        stopTransaction([] (JsonObject payload) {
            //Callback: Central System has answered. De-energize EV plug here.
            Serial.print(F("[main] Stopped OCPP transaction. EV plug de-energized\n"));
        });
    }
    ///Serial.printf("333\r\n");

    //... see ArduinoOcpp.h for more possibilities
}
