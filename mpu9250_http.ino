#include <Arduino.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_wdt.h"
#include "esp_int_wdt.h"
#include "esp_task_wdt.h"
#include <MPU9250_WE.h>
#include <SPI.h>
#include "WiFi.h"
#include <HTTPClient.h>
#include "global.h"



void MPU_GET(void* parameter) {
  esp_task_wdt_delete(NULL);
  myMPU9250.init();
  delay(1000);
  myMPU9250.autoOffsets();
  myMPU9250.disableGyrDLPF(MPU9250_BW_WO_DLPF_8800);  
  myMPU9250.setGyrRange(MPU9250_GYRO_RANGE_500);
  myMPU9250.setAccRange(MPU9250_ACC_RANGE_4G);
  myMPU9250.enableAccDLPF(false);
  myMPU9250.setAccDLPF(MPU9250_DLPF_6);
  myMPU9250.setMagOpMode(AK8963_CONT_MODE_100HZ);
  delay(1000);
  calibrate_MPU(Acc_bias, Gyro_bias);

  for (;;) {
    esp_task_wdt_init(10, false);
    xyzFloat gValue;
    xyzFloat gyr;
    for (int j = 0; j < N_set_gather; j++) {  //256 data points is 1 set and there are 4 sets
      for (int i = j * N_gather_size; i < (j + 1) * N_gather_size; i++) {
        if (micros() >= (lastMicros + MINIMUM_SAMPLING_DELAY_uSec)) {
          lastMicros = micros();
          timer1 = micros();
          gValue = myMPU9250.getGValues();
          gyr = myMPU9250.getGyrValues();

          Ax[i] = (gValue.x - Acc_bias[0]) * acc_factor;
          Ay[i] = (gValue.y - Acc_bias[1]) * acc_factor;
          Az[i] = (gValue.z - Acc_bias[2]) * acc_factor;
          Gx[i] = (gyr.x - Gyro_bias[0]) * gyr_factor;
          Gy[i] = (gyr.y - Gyro_bias[1]) * gyr_factor;
          Gz[i] = (gyr.z - Gyro_bias[2]) * gyr_factor;
          timer2 = micros();
         

        } else {
          i--;
        }
      }
      send_Flag = true;
      if (send_Flag) {
        send_Flag = false;
        vTaskResume(HTTP_SEND_h);
      } else {
        digitalWrite(led, HIGH);
        Serial.println("!!!OVERFLOW!!!");
        while (1) {
          delay(1000);
        }
      }

      esp_task_wdt_init(5, false); 
    }
  }
}

void HTTP_SEND(void* parameter) {
  vTaskSuspend(NULL);
  while (1) {

    for (int j = 0; j < N_set_gather; j++) {

      for (int i = j * N_gather_size; i < (j + 1) * N_gather_size; i++) {

        data +=
          +"/" + (String)(Ax[i]) + "," + (String)(Ay[i]) + "," + (String)(Az[i]) + "," + (String)(Gx[i]) + "," + (String)(Gy[i]) + "," + (String)(Gz[i]) + "\n";
      }
      if (WiFi.status() == WL_CONNECTED) {
        // Prepare your HTTP POST request
        http.begin(serverURL);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        String httpRequestData = "CHIPID=" + String(k_id) + "&mpudata=" + String(data);
        data = "";
        
        int httpCode = http.POST(httpRequestData);
      
        if (httpCode > 0) {
          if (httpCode == HTTP_CODE_OK) {
            String response = http.getString();
            Serial.println("HTTP POST request successful");
            Serial.println("Response: " + response);
          } else {
            Serial.println("HTTP POST request failed");
          }
        } else {
          Serial.println("HTTP POST request failed");
        }

        http.end();
      } else {
          Serial.println("!!WIFI DISCONNECT ERROR!!");
           connect_to_wifi(ssid, password);
        
      }
      vTaskSuspend(NULL);
    }


    esp_task_wdt_init(5, false);
  }
}

void setup() {
  Serial.begin(115200);
  // Serial.begin(115200);

  xTaskCreatePinnedToCore(MPU_GET, "MPU_GET", 10000, NULL, 1, &MPU_GET_h, 0);
  xTaskCreatePinnedToCore(HTTP_SEND, "HTTP_SEND", 10000, NULL, 1, &HTTP_SEND_h, 1);
  connect_to_wifi(ssid, password);

  //Deducing MpAcID of the ESP32 and stores in variable name device_ide
  chipid = ESP.getEfuseMac();  //The chip ID is essentially its MAC address(length: 6 bytes).
  sprintf(id_, "%04X", (uint16_t)(chipid >> 32));
  sprintf(id_1, "%08X\n", (uint32_t)chipid);
  strcat(kid, id_);
  strcat(kid, id_1);
  sprintf(k_id, "%c%c%c%c%c%c%c%c%c%c%c%c", kid[10], kid[11], kid[8], kid[9], kid[6], kid[7], kid[4], kid[5], kid[2], kid[3], kid[0], kid[1]);  //k_id is the ssid name of AP
  Serial.println(k_id);
  device_id = atoi(k_id);


}

void loop() {
}

void calibrate_MPU(float acc_bias[], float gyro_bias[]) {  

  const int MPU9250_MINIMUM_SAMPLING_DELAY_uSEC = 250;  //250 because max sampling rate of accelerometer is 4 khz

  for (int i = 0; i < 3; i++) {
    gyro_bias[i] = 0.0;
    acc_bias[i] = 0.0;
  }

  for (int i = 0; i < NUM_OF_CALIBRATION_SAMPLES; i++) {
    xyzFloat gValue = myMPU9250.getGValues();
    xyzFloat gyr = myMPU9250.getGyrValues();

    acc_bias[0] = acc_bias[0] + (gValue.x);
    acc_bias[1] = acc_bias[1] + (gValue.y);
    acc_bias[2] = acc_bias[2] + (gValue.z);
    gyro_bias[0] = gyro_bias[0] + (gyr.x);
    gyro_bias[1] = gyro_bias[1] + (gyr.y);
    gyro_bias[2] = gyro_bias[2] + (gyr.z);
    delayMicroseconds(MPU9250_MINIMUM_SAMPLING_DELAY_uSEC);  //delay because max sampling rate of accelerometer is 4 khz
  }

  for (int i = 0; i < 3; i++) {
    gyro_bias[i] = gyro_bias[i] / NUM_OF_CALIBRATION_SAMPLES;
    acc_bias[i] = acc_bias[i] / NUM_OF_CALIBRATION_SAMPLES;
  }
  acc_bias[2] = acc_bias[2] - 1;  //z axis -
  
}


void connect_to_wifi(const char* ssid, const char* password) {

  WiFi.begin(ssid, password);
  int wifi_counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
    if (wifi_counter == 10) {
      ESP.restart();
    }
    wifi_counter++;
  }
  Serial.println("connected to wifi");
}