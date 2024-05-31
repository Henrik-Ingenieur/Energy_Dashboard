#include <Arduino.h>

#include <WiFi.h>             // https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi
#include <PubSubClient.h>     // https://github.com/knolleary/pubsubclient
#include <ArduinoOTA.h>       // https://github.com/espressif/arduino-esp32/tree/master/libraries/ArduinoOTA

#include "_config.h" /* config / structs */

#include "_mqtt.h" /* MQTT Handling  */
#include "_Servo.h" /* Servo Handling  */

String data;
//char enter[] = "\r\n";
tstDtuValues stDtuValues; ///< Struct Deye Values


void setup() {
#if (ENABLE_UART == 1)
    Serial.begin(115200);
#endif
    delay(10);
    // We start by connecting to a WiFi network
#if (ENABLE_UART == 1)
    Serial.print("Connecting to ");
    Serial.println(ssid);
#endif
    WiFi.hostname(hostname);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);
    }
#if (ENABLE_UART == 1)
    Serial.println(WiFi.localIP());
#endif

    MQTT_vSetupMQTT_Register();
    ServoSetup();
    strcpy(msg_rx, "initial"); // prefill RX Msg
}

void loop() {
    MQTT_client.loop();     // needed in cyclic for MQTT
    vServoCyclicTick();     // trigger timer update at servo handler 
}

