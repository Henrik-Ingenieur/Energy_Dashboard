/**
    @file _mqtt.h
    @author your name (you@domain.com)
    @brief
    @version 0.1
    @date 2024-04-03

    @copyright Copyright (c) 2024
*/
#pragma once

#include <Arduino.h> // millis(), uint8_t
#include <string>    // std::string, std::stof
#include <ESPPubSubClientWrapper.h> //  v1.0.2  https://github.com/EFWob/ESPPubSubClientWrapper

#include "_config.h" // typedefs, MQTT Topics


typedef enum enTstServo {
    enServo_lr,   //< lower range
    enServo_ur,   //< upper range
    enServo_pos,  //< pointer position in range ur - lr
    enServo_deg,  //< pointer position in degrees
    enServo_delay,//< delay to move 
    enServo_sz,   //< stepsite to get to the next position
    enServo_swp,  //< sweep dis/en-able
    enMqttData_Load_Power,     //< watts consumed in-house
    enMqttData_Battery_level,  //< battery charging level in percent
    enMqttData_cur_pv_prod     //< sweep dis/en-able
} tenTstServo;

typedef enum tenDeye {
    enDeye_START_INVALID_,
    enDeye_PV1,
    enDeye_PV2,
    enDeye_DCSum,
    enDeye_END_INVALID_
} tenDeye;

typedef enum tenShelly {
    enShelly_START_INVALID_,
    enPhase0,
    enPhase1,
    enPhase2,
    enShelly_END_INVALID_
} tenShelly;

long lastMsg = 0; // millis() boot
extern volatile uint8_t u8Blink;
extern char msg[50];
extern char msg_rx[11];
extern long msg_rx_len;
int _Load_Power;    //< global: last Load Power from MQTT
int _Battery_level;    //< global: last Battery Power info from MQTT
int _cur_pv_prod;    //< global: last cur_pv_prod from MQTT

extern tstDtuValues stDtuValues;
extern void Servotest(tenTstServo msgType, int iVal);
extern void ServoNewLoadPower(int LoadPwr);
extern void ServoNewCurPvPower(int PvPwr);
extern void ServoNewBatLvl(int BatLvl);

//MQTT messages
char msg[50];
char msg_rx[11];
long msg_rx_len;

ESPPubSubClientWrapper MQTT_client(mqtt_server, mqtt_servPort);
// LOCAL

void MQTT_Callback_connectSuccess(uint16_t count);
void MQTT_Callback_disconnect(uint16_t count);
void MQTT_RxCallback_DtuAcPower(tenTstServo msgType, byte *payload, unsigned int length);
void callback(char* topic, byte* payload, unsigned int length);

void MQTT_Pub_RX_msg(void);
void MQTT_Publish(const char *pcsTopic, char *pcsMsg);
void vNewMqttData(void);

/********************************************************************************************************************
       MQTT
 *********************************************************************************************************************/

/**
    @brief Setup MQTT Callbacks

*/
void MQTT_vSetupMQTT_Register(void)
{
#if (ENABLE_UART == 1)
      Serial.println("registering at MQTT");
#endif

    MQTT_client.setCallback(callback);
    MQTT_client.connect("Dashboard", mqtt_user, mqtt_pass);
    MQTT_client.subscribe(OpenDTU_ac_power);
    MQTT_client.onConnect(MQTT_Callback_connectSuccess); // Subscribe MQTT connect event
    MQTT_client.onDisconnect(MQTT_Callback_disconnect);

#if 0
    MQTT_client.on("lightOff", [](char* topic, byte * payload, unsigned int length) { // Subscribe MQTT
        u8Blink = 0;
        LED_OFF();
    });
    MQTT_client.on("lightOn", [](char* topic, byte * payload, unsigned int length) { // Subscribe MQTT
        u8Blink = 1;
        LED_ON();
    });
#endif
    MQTT_client.on("disconnect", [](char *topic, byte * payload, unsigned int length) { // Subscribe MQTT
        MQTT_client.disconnect();
    });

    // power act
    MQTT_client.on( MQTT_TST_Servo_lr,    [](char *topic, byte * payload, unsigned int length) { MQTT_RxCallback_DtuAcPower(enServo_lr,    payload, length);});
    MQTT_client.on( MQTT_TST_Servo_ur,    [](char *topic, byte * payload, unsigned int length) { MQTT_RxCallback_DtuAcPower(enServo_ur,    payload, length);});
    MQTT_client.on( MQTT_TST_Servo_pos,   [](char *topic, byte * payload, unsigned int length) { MQTT_RxCallback_DtuAcPower(enServo_pos,   payload, length);});
    MQTT_client.on( MQTT_TST_Servo_deg,   [](char *topic, byte * payload, unsigned int length) { MQTT_RxCallback_DtuAcPower(enServo_deg,   payload, length);});
    MQTT_client.on( MQTT_TST_Servo_delay, [](char *topic, byte * payload, unsigned int length) { MQTT_RxCallback_DtuAcPower(enServo_delay, payload, length);});
    MQTT_client.on( MQTT_TST_Servo_sz,    [](char *topic, byte * payload, unsigned int length) { MQTT_RxCallback_DtuAcPower(enServo_sz,    payload, length);});
    MQTT_client.on( MQTT_TST_Servo_swp,   [](char *topic, byte * payload, unsigned int length) { MQTT_RxCallback_DtuAcPower(enServo_swp,   payload, length);});

    MQTT_client.on( MQTT_Load_Power,      [](char *topic, byte * payload, unsigned int length) { MQTT_RxCallback_DtuAcPower(enMqttData_Load_Power,    payload, length);});
    MQTT_client.on( MQTT_Battery_level,   [](char *topic, byte * payload, unsigned int length) { MQTT_RxCallback_DtuAcPower(enMqttData_Battery_level, payload, length);});
    MQTT_client.on( MQTT_cur_pv_prod,     [](char *topic, byte * payload, unsigned int length) { MQTT_RxCallback_DtuAcPower(enMqttData_cur_pv_prod,   payload, length);});

    //   MQTT_Shelly_POWER_3_Register_String, [](char *topic, byte * payload, unsigned int length) { // Subscribe Shelly Power3
    //     MQTT_RxCallback_vShelly_Power(enPhase2, payload, length);
    //   },
    //   mqtt_Shelly_qos);
}
/********************************************************************************************************************/

// just publish the RX Buffer
void MQTT_Pub_RX_msg()
{
    const char csOutTopic1[] = "outTopic1";

    snprintf(msg, 50, "RxMSG #%.10s#%d#%d", msg_rx, msg_rx_len, stDtuValues.i16Dtu_power);
#if (ENABLE_UART == 1)
    Serial.print("Publish message: ");
    Serial.println(msg);
#endif
    MQTT_Publish(csOutTopic1, msg);
}

void MQTT_Publish(const char *pcsTopic, char *pcsMsg)
{
    if (MQTT_client.connected())
        MQTT_client.publish(pcsTopic, pcsMsg);
}


/*******************************************************************************************************************/
/**
    @brief MQTT connect event

    @param count  Count of connections, every reconnect will increment this
*/
void MQTT_Callback_connectSuccess(uint16_t count)
{
#if (ENABLE_UART == 1)
    Serial.println("Connected to MQTT-Broker!\nThis is connection nb: ");
    Serial.println(count);
#endif
    lastMsg = millis();
    MQTT_client.publish("outTopic", "Hello from Energy-Dashboard");
}

/**
   @brief Disconnect callback

   @param count
*/
void MQTT_Callback_disconnect(uint16_t count)
{
  MQTT_client.disconnect();
  //  ServoSetPos_DEYE(0);
  //  myservo_Overall.write(0);
}

/********************************************************************************************************************/

/**
    @brief Callback for Mqtt data packages
    payload only valid in callback

    @param payload   Data received,  only valid in callback
    @param length  Lenght of data received
*/
void MQTT_RxCallback_DtuAcPower(tenTstServo msgType, byte *payload, unsigned int length)
{
    boolean boDataUpdate = true;
    memcpy(&msg_rx, payload, length); // hier ok, String
    msg_rx_len = length;              // data to global buff
    // Convert the payload
    payload[length] = '\0'; // Add a NULL to the end of the char* to make it a string.      kommt als   0.0
    int aNumber = atoi((char *)payload);

    // process the given data
    switch(msgType){
        case enMqttData_Load_Power:
            _Load_Power = aNumber;
            ServoNewLoadPower(aNumber);
            break;
        case enMqttData_Battery_level:
            _Battery_level = aNumber;
            ServoNewBatLvl(aNumber);
            break;
        case enMqttData_cur_pv_prod:
            _cur_pv_prod = aNumber;
            ServoNewCurPvPower(aNumber);
            break;
#ifdef SERVO_TEST
        case enServo_lr:
        case enServo_ur:
        case enServo_pos:
        case enServo_deg:
        case enServo_delay:
        case enServo_sz:
        case enServo_swp:
            Servotest(msgType, aNumber);
            break;
#endif
        default:
            boDataUpdate = false;
            break;
    };
    if (boDataUpdate)
        vNewMqttData();
}

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

/*******************************************************************************************************************/
/*******************************************************************************************************************/
