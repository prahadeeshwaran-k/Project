#include "WiFiSetup.h"

// Define the variables here
const char *ssid = "Note";
const char *password = "00000000";

void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("Connected to AP successfully!");
}

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("\n\nDisconnected from WiFi access point");
  Serial.print("WiFi lost connection. Reason: ");
  // Translate reason code to human-readable text
  switch (info.wifi_sta_disconnected.reason) {
    case WIFI_REASON_UNSPECIFIED:
      Serial.println("Reason: Unspecified");
      break;
    case WIFI_REASON_AUTH_EXPIRE:
      Serial.println("Reason: Authentication expired");
      break;
    case WIFI_REASON_AUTH_FAIL:
      Serial.println("Reason: Authentication failed");
      break;
    case WIFI_REASON_NO_AP_FOUND:
      Serial.println("Reason: No Access Point found");
      break;
    case WIFI_REASON_BEACON_TIMEOUT:
      Serial.println("Reason: Beacon timeout");
      break;
    default:
      Serial.println("Reason: Unknown");
      break;
  }
  Serial.println("Reconnect to WiFi from Event...");
  WiFi.begin(ssid, password);
}

void WiFiSetup() {
  Serial.begin(115200);
  vTaskDelay(500);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("WiFi Started");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Attempting to connect to WiFi...");
    WiFi.begin(ssid, password);

    // Wait until connected or timeout
    int retryCount = 0;
    const int maxRetries = 20;  // Adjust this as needed for a suitable timeout period
    while (WiFi.status() != WL_CONNECTED && retryCount < maxRetries) {
      Serial.print('.');
      vTaskDelay(500 / portTICK_PERIOD_MS);  // 500ms delay
      retryCount++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nConnected to WiFi");
      Serial.println("WiFi Setup Task: WiFi is connected & events are created");

      // Create the Events for WiFi
      WiFi.onEvent(WiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
      WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
      WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

    } else {
      Serial.println("\nFailed to connect to WiFi. Retrying...");
      WiFi.disconnect(true);                  // Disconnect before trying again
      vTaskDelay(3000 / portTICK_PERIOD_MS);  // Wait for 3 seconds before retrying
    }
  }
}
