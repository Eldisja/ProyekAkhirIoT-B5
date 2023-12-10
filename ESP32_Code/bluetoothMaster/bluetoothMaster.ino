#include <ESP32Servo.h>
#include <BluetoothSerial.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>


#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;

#define BT_DISCOVER_TIME 10000
#define IRSensor 18 //IR Sensor pin
#define ServoOut 14
#define ServoVcc 12
#define ServoGnd 13
static bool btScanAsync = true;
static bool btScanSync = true;
String nearestAddress;
TimerHandle_t bluetoothOneShotTimer;

String registeredDevices = "ba:98:76:f4:08:7d";
Servo Servo1;

// Function to check if a specific word is present anywhere in a string
bool isWordPresent(String input, String targetWord) {
  int index = input.indexOf(targetWord);
  return (index != -1);
}

void openGate(){
  Serial.println("Access Permitted");
  Serial.println("Opening the gate!");
  Servo1.write(180);
  if(digitalRead(IRSensor)){
    vTaskDelay(pdMS_TO_TICKS(700)); // 100ms delay
    Servo1.write(90);
  }
}
void btAdvertisedDeviceFound(BTAdvertisedDevice *pDevice) {
    Serial.printf("Found a device asynchronously: %s\n", pDevice->toString().c_str());
    String receivedMessage = Serial.readStringUntil('\n');
    if (pDevice->getRSSI() > -50) {
        nearestAddress = pDevice->getAddress().toString();
        Serial.println("Nearest Device MAC Address : " + nearestAddress);

        if (nearestAddress.equals(registeredDevices)) {
            openGate();
            delay(100);
        } else if (isWordPresent(receivedMessage, "Permitted")) {
            openGate();
        } else {
            Serial.println("Access Denied");
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }
}

void bluetoothTimerCallback(TimerHandle_t xTimer) {
    uint32_t startTime = millis();
    while (!digitalRead(IRSensor)) {  // Timeout after 10 seconds
        String receivedMessage = Serial.readStringUntil('\n');
        Serial.println("Object detected");
        if (btScanAsync) {
            Serial.print("Starting discoverAsync...");
            if (SerialBT.discoverAsync(btAdvertisedDeviceFound)) {
                delay(1000);
                SerialBT.discoverAsyncStop();
            } else {
                Serial.println("Error on discoverAsync");
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void IRCheck(void *pvParameters) {
    while (1) {
        //if (!digitalRead(IRSensor)) {
          if(!digitalRead(IRSensor)){
            String receivedMessage = Serial.readStringUntil('\n');
            if(isWordPresent(receivedMessage, "Permitted")){
              openGate();
            }
            if (bluetoothOneShotTimer == NULL) {
                bluetoothOneShotTimer = xTimerCreate(
                    "Bluetooth One Shot",        // Timer name
                    pdMS_TO_TICKS(500),          // Timer interval
                    pdFALSE,                     // One-shot timer
                    NULL,                        // Timer parameter
                    bluetoothTimerCallback       // Timer callback function
                );
                xTimerStart(bluetoothOneShotTimer, portMAX_DELAY); // Start the timer
            }
          }else {
            // Object not detected, stop the timer
            if (bluetoothOneShotTimer != NULL) {
                xTimerStop(bluetoothOneShotTimer, portMAX_DELAY);
                xTimerDelete(bluetoothOneShotTimer, portMAX_DELAY);
                bluetoothOneShotTimer = NULL;
            }
          }
          vTaskDelay(pdMS_TO_TICKS(100)); // 100ms delay
    }
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32 Slave"); // Bluetooth device name
  Serial.println("The device started, now you can pair it with Bluetooth!");
  pinMode(15, OUTPUT);
  pinMode(ServoVcc, OUTPUT);
  pinMode(ServoGnd, OUTPUT);
  Servo1.attach(ServoOut);
  digitalWrite(ServoVcc, HIGH);
  digitalWrite(ServoGnd, LOW);
  digitalWrite(15, HIGH);
  pinMode(IRSensor, INPUT);

 
  xTaskCreate(IRCheck, "IRCheck", 1024, NULL, 1, NULL);
}

void loop() {
    // Your loop code here, if needed
}
