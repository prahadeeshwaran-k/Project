#include <EmonLib.h>
#include <ArduinoLog.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include "WiFiSetup.h"


QueueHandle_t queueHandler;

// Define MQTT parameters
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
const String id = "MQTT-ESP32";

// Define the Last Will message parameters
const char* willTopic = "prahadeesh/will";
const char* willMessage = "disconnected";
const int willQoS = 1;
const boolean willRetain = true;

const int keepAliveInterval = 45;  // Keep-alive interval in seconds

WiFiClient espClient;
PubSubClient client(espClient);

//----------------------------------------------------------! MqttConnect !----------------------------------------------------------

void MqttConnect() {
  client.setServer(mqtt_server, mqtt_port);  // Set the MQTT-server
  client.setKeepAlive(keepAliveInterval);    // Set the keep-alive interval
  String clientId = id;
  Log.notice(F("MqttConnect                           : %s" CR), "Attempting MQTT connection...");
  do {
    client.connect(clientId.c_str(), willTopic, willQoS, willRetain, willMessage);
    Log.notice(F("MqttConnect                           : %s" CR), "Try - TO Connect");
    // TODO: add a led status to indicate connection in BLINK
    vTaskDelay(3000 / portTICK_PERIOD_MS);
  } while (!client.connected());

  if (client.connected()) {
    Log.notice(F("MqttConnect                           : %s\n" CR), "MQTT-connected");
    Serial.println("|-------------------------------------------------------------------------|\n");
  }
}

//----------------------------------------------------------! MqttReconnect !----------------------------------------------------------

void MqttReconnect() {
  do {
    Log.notice(F("MqttReconnect                         : %s" CR), "MQTT Re-connect...");
    String clientId = id;
    client.connect(clientId.c_str(), willTopic, willQoS, willRetain, willMessage);
    if (client.connected()) {
      Log.notice(F("MqttReconnect                         : %s" CR), "Connected");
      // TODO: add a led status to indicate successful connection
    } else {
      Log.notice(F("MqttReconnect                         : %s" CR), client.state());
      delay(5000 / portTICK_PERIOD_MS);
      // TODO: add a led status to indicate failed connection attempt
    }
  } while (!client.connected());
}

//----------------------------------------------------------! MqttPublish_Task !----------------------------------------------------------

void MqttPublish_Task(void* pvParameter) {  //AKA Publish
  while (1) {
    BaseType_t xStatus;
    char data[80];  // Buffer to store data received from queue
    xStatus = xQueueReceive(queueHandler, &data, portMAX_DELAY);

    if (xStatus == pdPASS) {
      if (!client.connected()) {
        MqttReconnect();
      } else {
        client.publish("prahadeesh/Test", data);
        Log.notice(F("MQTT Published Message                : %s" CR), "Publish-OK\n");
      }
    } else {
      Log.error(F("MqttPublish_Task                        : %s" CR), "Data not received");
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    client.loop();
  }
}

//----------------------------------------------------------! Sensor_Task !----------------------------------------------------------

void Sensor_Task(void* pvParameter) {
  BaseType_t xStatus;
  while (1) {
    StaticJsonDocument<80> doc;
    char output[80];

    float temp = random(1, 20);
    float pressure = random(20, 40);
    float humidity = random(40, 60);
    float gas = random(60, 80);

    doc["w"] = temp;
    doc["v"] = pressure;
    doc["c"] = humidity;
    doc["p"] = gas;

    serializeJson(doc, output);
    xStatus = xQueueSend(queueHandler, &output, portMAX_DELAY);
    if (xStatus == pdPASS) {
      Log.notice(F("Sensor_Task                           : %s" CR), output);
    } else {
      Log.error(F("Sensor_Task                           : %s" CR), "Data not sent");
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

//----------------------------------------------------------! Setup Function !----------------------------------------------------------

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial && !Serial.available()) {}
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);

  vTaskDelay(500);
  WiFiSetup();
  Log.notice(F("Setup Function                        : %s" CR), "Wifi-Connected");
  MqttConnect();

  queueHandler = xQueueCreate(
    5,                 // Queue length
    sizeof(char) * 80  // Queue item size
  );
  if (queueHandler != NULL) {
    xTaskCreate(Sensor_Task, "Sensor_Task", 1024 * 5, NULL, 2, NULL);
    xTaskCreate(MqttPublish_Task, "MqttPublish_Task", 1024 * 5, NULL, 2, NULL);
  } else {
    Log.notice(F("Setup Function                        : %s" CR), "Queue creation failed");
  }
}

//----------------------------------------------------------! Loop Function !----------------------------------------------------------

void loop() {
  vTaskDelete(NULL);
}