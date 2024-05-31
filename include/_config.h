/**
   @file _config.h
   @author your name (you@domain.com)
   @brief
   @version 0.1
   @date 2024-04-03

   @copyright Copyright (c) 2024

*/
#pragma once

#define ENABLE_DEBUG 1
#define ENABLE_UART 1
#define ENABLE_TEST_Pub_DEYE 1
#define ENABLE_TEST_Pub_All 1
#define SWEEPTEST 0

// Update these with values suitable for your network.

const char *ssid = "PauLANer";
const char *password = "Z7F3#awonbs29%=!.?";
const char *hostname = "Energy_Dashboard";
const char *mqtt_server = "192.168.79.164";
const char *mqtt_user = "Henning";
const char *mqtt_pass = "tHiuv1nem#m13L4";
const uint16_t mqtt_servPort = 1883;
const uint8_t mqtt_Shelly_qos = 0; /* only "0" supported */
const uint8_t mqtt_Deye_qos = 0;   /* only "0" supported */

#define UART_BAUD 115200

#define MQTT_DEYE_TIMEOUT 650   /* time[sec] until value gets invalid, intervall max = 360sec */
#define MQTT_SHELLY_TIMEOUT 100 /* time[sec] until value gets invalid */

#define MQTT_DEYE_DC_POWER_TOTAL_Register_String "deye/dc/total_power"
//#define OpenDTU_ac_power "OpenDTU/ac/power"
#define OpenDTU_ac_power "$SYS/broker/uptime"
// #define MQTT_DEYE_DC_POWER_DAY_Register_String "deye/day_energy"

// Test commands to check Dashboard
#define MQTT_TST_Servo_lr     "test/dashboard/lr"
#define MQTT_TST_Servo_ur     "test/dashboard/ur"
#define MQTT_TST_Servo_pos    "test/dashboard/pos"
#define MQTT_TST_Servo_deg    "test/dashboard/deg"
#define MQTT_TST_Servo_delay  "test/dashboard/delay"
#define MQTT_TST_Servo_sz     "test/dashboard/sz"
#define MQTT_TST_Servo_swp    "test/dashboard/swp"
#define MQTT_Load_Power       "homeassistant/sensor/Load_power"
#define MQTT_Battery_level    "homeassistant/sensor/Battery_level"
#define MQTT_cur_pv_prod      "homeassistant/sensor/current_pv_prod_total"

#define MQTT_Shelly_POWER_1_Register_String "shellies/shellyem3-C8C9A316D608/emeter/0/power"
#define MQTT_Shelly_POWER_2_Register_String "shellies/shellyem3-C8C9A316D608/emeter/1/power"
#define MQTT_Shelly_POWER_3_Register_String "shellies/shellyem3-C8C9A316D608/emeter/2/power"

#define MQTT_Shelly_Total_1_Register_String "shellies/shellyem3-C8C9A316D608/emeter/0/total"
#define MQTT_Shelly_TotalRet_1_Register_String "shellies/shellyem3-C8C9A316D608/emeter/0/total_returned"
#define MQTT_Shelly_Total_2_Register_String "shellies/shellyem3-C8C9A316D608/emeter/1/total"
#define MQTT_Shelly_TotalRet_2_Register_String "shellies/shellyem3-C8C9A316D608/emeter/1/total_returned"
#define MQTT_Shelly_Total_3_Register_String "shellies/shellyem3-C8C9A316D608/emeter/2/total"
#define MQTT_Shelly_TotalRet_3_Register_String "shellies/shellyem3-C8C9A316D608/emeter/2/total_returned"

/*
   ESP8266     https://docs.espressif.com/projects/esptool/en/latest/esp8266/advanced-topics/boot-mode-selection.html#boot-mode

  D0 = 16
  D1 = 5
  D2 = 4
  D3 = 0    Low->Serial / High->Normal
  D4 = 2    BOOT -> VCC/ OC
  D5 = 14
  D6 = 12
  D7 = 13
  D8 = 15   BOOT GND
  RX = 3
  TX = 1
*/

/**
   @brief Struct with values from Deye
*/
typedef struct tstDtuValues
{
  uint16_t u16TimeoutSec; ///<  Sec Countdown bis "invalid"/ Missing value
  int16_t i16Dtu_power;   ///< akt Power Deye  16 bit -> -32768..32767
} tstDtuValues;

/**
   @brief Struct with values from Shelly
*/
typedef struct tstPowers
{
  uint16_t u16TimeoutSec; ///<  Sec Countdown bis "invalid"/ Missing value

  int iShellyPwr1;        ///< akt Power L1 [W]
  int iShellyPwr2;        ///< akt Power L2 [W]
  int iShellyPwr3;        ///< akt Power L3 [W]
  int iShellyPwrCombined; ///< akt Power Summe L1-3 [W]
  uint8_t u8Shelly_val;

  // Angabe in Wh
  long lTotal_1; /* 32 bit  [Wh]*/
  long lTotalRet_1;
  long lTotal_2;    //[Wh]
  long lTotalRet_2; // nur hier Solarzelle [Wh]
  long lTotal_3;    // [Wh]
  long lTotalRet_3;

} tstPowers;
