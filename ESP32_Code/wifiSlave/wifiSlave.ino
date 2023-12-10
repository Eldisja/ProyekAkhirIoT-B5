#include "config.h"
#include "my_wifi.h"
#include "wifi_communicator.h"

#include <MyButton.h>

#define ENABLE_DEBUG /* <-- Commenting this line will remove any trace of debug printing */
#include <MacroDebugger.h>

// Task Handle
TaskHandle_t UARTHandle = NULL;
TaskHandle_t wifiHandle = NULL;

// Communication messages
char incoming_msg[MAX_BUFFER_LEN] = {0};
char response[MAX_BUFFER_LEN] = {0};
String input;
bool accessDeniedSent = false;  // Flag to track whether the "Access Denied" response has been sent

/* A collection of random responses to send when the button is clicked */
#define NUM_RANDOM_RESPONSES    1
char *responses[NUM_RANDOM_RESPONSES] = {
  "Denied!"
};

void receiveUART(void *pvParameters){
  while(1){
    input = Serial.readStringUntil('\n');
    Serial.println(input);
    if(isWordPresent(input, "Access Denied")){
      if(wifiHandle == NULL){
        xTaskCreatePinnedToCore(WifiCommTask, "WifiComm Task", 10240, NULL, 5, &wifiHandle, 1);
      }else{
        vTaskResume(wifiHandle);
      }
    }
  }
}

void WifiCommTask(void *pvParameters){
  vTaskSuspend(UARTHandle);
  while(1){
    if (eTaskGetState(UARTHandle) == eRunning) {
      vTaskSuspend(wifiHandle);
    }
    // if we lost connection, we attempt to reconnect (blocking)
    if(!is_client_connected()){ 
      connect_client(); 
      }
    bool received = get_message(incoming_msg);
    if(isWordPresent(input, "Access Denied")){
        // Choose a random response to send back
        strncpy(response, responses[0], MAX_BUFFER_LEN);
        send_message(response);
        memset(response, 0, MAX_BUFFER_LEN);
        DEBUG_I("Sent: %s", responses[0]);
        accessDeniedSent = true;  // Set the flag to true once the response is sent
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    if(received){
      DEBUG_I("Received: %s", incoming_msg);
      uint8_t start = 0;

      if(incoming_msg[0] == 'A'){
        sprintf(response, "%s", ACK);
        start++;
      }

      //switch the number and do the appropriate action
      switch(incoming_msg[start]){
        case 'y':
          vTaskResume(UARTHandle);
          Serial.println("Permitted");
          accessDeniedSent = false;  // Set the flag to true once the response is sent
          vTaskDelay(1000/ portTICK_PERIOD_MS);
          // Check if UARThandle is resumed, and suspend wifiHandle accordingly
          break;

        default:
        case 'n':
          Serial.println("Denied");
          break;
      }

      // If start is bigger than 0, then we have to acknowledge the reception
      if(start > 0){
        send_message(response);
        // Clear the response buffer
        memset(response, 0, MAX_BUFFER_LEN);
      }
    }
  }  
  vTaskDelete(NULL);
}


// Function to check if a specific word is present anywhere in a string
bool isWordPresent(String input, String targetWord) {
  int index = input.indexOf(targetWord);
  return (index != -1);
}

void setup(){
  DEBUG_BEGIN();
  
  setup_wifi();
  Serial.println("Waiting for server online");
  setup_wifi_communicator();
  xTaskCreatePinnedToCore(receiveUART, "UART Receive", 4096, NULL, 4, &UARTHandle, 1);

  DEBUG_I("Done setting up!");
}

void loop(){


   
}
