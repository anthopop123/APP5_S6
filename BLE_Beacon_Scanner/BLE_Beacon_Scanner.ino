#include <HTTPClient.h>

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEEddystoneURL.h>
#include <BLEEddystoneTLM.h>
#include <BLEBeacon.h>
#include <WiFi.h>
#include <Arduino_JSON.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
const char* ssid = "Techcave";
const char* password = "Sodastream8";

const char* serverName = "http://192.168.2.22:4567/idlist";
int scanTime = 5; //In seconds
BLEScan *pBLEScan;
WiFiClient client;
HTTPClient http;
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
      if (advertisedDevice.haveManufacturerData() == true)
      {
        std::string strManufacturerData = advertisedDevice.getManufacturerData();

        uint8_t cManufacturerData[100];
        strManufacturerData.copy((char *)cManufacturerData, strManufacturerData.length(), 0);

        if (strManufacturerData.length() == 25 && cManufacturerData[0] == 0x4C && cManufacturerData[1] == 0x00)
        {
          BLEBeacon oBeacon = BLEBeacon();
          oBeacon.setData(strManufacturerData);
          Serial.printf("ID: %d\n", oBeacon.getManufacturerId());
          if(WiFi.status()== WL_CONNECTED){
          
            // Your Domain name with URL path or IP address with path
            http.begin(client,serverName);
            http.addHeader("Content-Type", "application/json");
            String httpRequestData = "{\"id\":\""+String(oBeacon.getProximityUUID().toString().c_str())+"\",\"piece\":\"Salon\"}";
            Serial.println(httpRequestData);
            int httpResponseCode = http.POST(httpRequestData);

            // If you need an HTTP request with a content type: text/plain
            //http.addHeader("Content-Type", "text/plain");
            //int httpResponseCode = http.POST("Hello, World!");
          
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
              
            // Free resources
            http.end();
          }
          else{
            Serial.println("WiFi Disconnected");
          }
        }
      }

      uint8_t *payLoad = advertisedDevice.getPayload();
      // search for Eddystone Service Data in the advertising payload
      // *payload shall point to eddystone data or to its end when not found
      const uint8_t serviceDataEddystone[3] = {0x16, 0xAA, 0xFE}; // it has Eddystone BLE UUID
      const size_t payLoadLen = advertisedDevice.getPayloadLength();
      uint8_t *payLoadEnd = payLoad + payLoadLen - 1; // address of the end of payLoad space
      while (payLoad < payLoadEnd) {
        if (payLoad[1] == serviceDataEddystone[0] && payLoad[2] == serviceDataEddystone[1] && payLoad[3] == serviceDataEddystone[2]) {
          // found!
          payLoad += 4;
          break;
        }
        payLoad += *payLoad + 1;  // payLoad[0] has the field Length
      }

      if (payLoad < payLoadEnd) // Eddystone Service Data and respective BLE UUID were found
      {
        if (*payLoad == 0x10)
        {
          Serial.println("Found an EddystoneURL beacon!");
          BLEEddystoneURL foundEddyURL = BLEEddystoneURL();
          uint8_t URLLen = *(payLoad - 4) - 3;  // Get Field Length less 3 bytes (type and UUID) 
          foundEddyURL.setData(std::string((char*)payLoad, URLLen));
          std::string bareURL = foundEddyURL.getURL();
          if (bareURL[0] == 0x00)
          {
            // dumps all bytes in advertising payload
            Serial.println("DATA-->");
            uint8_t *payLoad = advertisedDevice.getPayload();
            for (int idx = 0; idx < payLoadLen; idx++)
            {
              Serial.printf("0x%02X ", payLoad[idx]);
            }
            Serial.println("\nInvalid Data");
            return;
          }

          Serial.printf("Found URL: %s\n", foundEddyURL.getURL().c_str());
          Serial.printf("Decoded URL: %s\n", foundEddyURL.getDecodedURL().c_str());
          Serial.printf("TX power %d\n", foundEddyURL.getPower());
          Serial.println("\n");
        } 
        else if (*payLoad == 0x20)
        {
          Serial.println("Found an EddystoneTLM beacon!");
 
          BLEEddystoneTLM eddystoneTLM;
          eddystoneTLM.setData(std::string((char*)payLoad, 14));
          Serial.printf("Reported battery voltage: %dmV\n", eddystoneTLM.getVolt());
          Serial.printf("Reported temperature: %.2f°C (raw data=0x%04X)\n", eddystoneTLM.getTemp(), eddystoneTLM.getRawTemp());
          Serial.printf("Reported advertise count: %d\n", eddystoneTLM.getCount());
          Serial.printf("Reported time since last reboot: %ds\n", eddystoneTLM.getTime());
          Serial.println("\n");
          Serial.print(eddystoneTLM.toString().c_str());
          Serial.println("\n");
        }
      }
    }
};

AsyncWebServer server(80);
const int output = 18;
void setup()
{
  Serial.begin(115200);

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99); // less or equal setInterval value
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.print("ESP IP Address: http://");
  Serial.println(WiFi.localIP());
  pinMode(output, OUTPUT);
  digitalWrite(output, LOW);
  //Receive an HTTP GET request
  server.on("/on", HTTP_GET, [] (AsyncWebServerRequest *request) {
    digitalWrite(output, HIGH);
    request->send(200, "text/plain", "ok");
    
  });

  // Receive an HTTP GET request
  server.on("/off", HTTP_GET, [] (AsyncWebServerRequest *request) {
    digitalWrite(output, LOW);
    request->send(200, "text/plain", "ok");
  });
  server.begin();
}

void loop()
{
  // put your main code here, to run repeatedly:
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory
  delay(2000);
}
