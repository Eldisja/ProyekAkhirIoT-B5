/* 
 * -----------------------------------------------------------------------------
 * Example: Two way communication between ESP32 and Python using WIFI
 * -----------------------------------------------------------------------------
 * Author: Radhi SGHAIER: https://github.com/Rad-hi
 * -----------------------------------------------------------------------------
 * Date: 07-05-2023 (7th of May, 2023)
 * -----------------------------------------------------------------------------
 * License: Do whatever you want with the code ...
 *          If this was ever useful to you, and we happened to meet on 
 *          the street, I'll appreciate a cup of dark coffee, no sugar please.
 * -----------------------------------------------------------------------------
 */

#ifndef __CONFG_H___
#define __CONFG_H___


/* Pins definitions */

#define IRSensor 18 //IR Sensor pin
#define ServoOut 14
#define ServoVcc 12
#define ServoGnd 13

/* Communication params */
#define BT_DISCOVER_TIME 10000
#define ACK                         "A" // acknowledgment packet
#define QUEUE_LEN                   5
#define MAX_BUFFER_LEN              32

/* WiFi params */
#define WIFI_SSID                   "Cappucino"
#define WIFI_PASSWORD               "milkshake5623"

/* Socket */
#define SERVER_ADDRESS              "192.168.18.47"
#define SERVER_PORT                 11111

#endif // __CONFG_H___
