

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
