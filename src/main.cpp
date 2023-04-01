#include <WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include <DHT.h>
#include "ssh.hpp"
#include "esp_wpa2.h"

const unsigned int configSTACK = 40960;
TaskHandle_t sshHandle = NULL;

const char* ssid = "IITBhilai";
#define EAP_ID "gaddeyhemanth"
#define EAP_USERNAME "gaddeyhemanth"
#define EAP_PASSWORD "GAmaGey@gDRY@2"

#define DHTPin 32

DHT dht(DHTPin, DHT22); 

float temperature = 0.0;
char cmd[60];
void sshTask(void* pvParameter) {
  SSH ssh{};

  Serial.println("SSH Connecting to server...");

  ssh.connectWithPassword("10.1.81.131", "gagan","Bhilai@IIT2022");

  if (ssh.isConnected) {
    Serial.println("SSH is connected!\n");
    Serial.println("Starting to send Commands");

    while(true){
      if(WiFi.status() != WL_CONNECTED) {
          Serial.println(" wifi got disconnected");
          Serial.println("Close ssh connection");
          ssh.end();
          Serial.println("Kill ssh task");
          vTaskDelete(NULL);
          Serial.println("Restarting ESP32");
          delay(500);
          ESP.restart();
      }

      temperature = dht.readTemperature();
      snprintf(cmd, sizeof(cmd), "./TemperatureLogs/logger.sh %.1f", temperature);
      Serial.println(cmd);
      ssh.sendCommand(cmd);
      delay(15000);
    }
  }
  else {
    Serial.println("SSH connection failed.");
  }

  Serial.println("Close ssh connection");
  ssh.end();
  Serial.println("Kill ssh task");
  vTaskDelete(NULL);
  ESP.restart();
}

void setup(void) {
  Serial.begin(115200);
  delay(10);
  
  // WPA2 enterprise connection authentication
  WiFi.disconnect(true);      
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_ID, strlen(EAP_ID));
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_USERNAME, strlen(EAP_USERNAME));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
  esp_wifi_sta_wpa2_ent_enable();

  WiFi.begin(ssid);

  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println("\nConnected to the WiFi network");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Subnet Mask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway IP: ");
  Serial.println(WiFi.gatewayIP()); 
  Serial.print("DNS 1: ");
  Serial.println(WiFi.dnsIP(0));
  Serial.print("DNS 2: ");
  Serial.println(WiFi.dnsIP(1));
  
  pinMode(DHTPin, INPUT);
  dht.begin();

  xTaskCreatePinnedToCore(sshTask, "ctl", configSTACK, NULL,
                          (tskIDLE_PRIORITY + 3), &sshHandle,
                          portNUM_PROCESSORS - 1);
}

void loop(void) {
  delay(1);
}
