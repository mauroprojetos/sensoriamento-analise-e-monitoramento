/************************** Inclusão das Bibliotecas **************************/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <Wire.h>
#include "Adafruit_MCP9808.h"

/********************************** WiFi **************************************/

#include "user_config.h"
#include "user_config_override.h"

/******************************* Broker MQTT **********************************/

const char* BROKER_MQTT = "192.168.0.20";
int BROKER_PORT         = 1883;

/**************************** Variaveis globais *******************************/

unsigned long previousMillis = 0;
char temp[4];
const char* MQTT_TOPIC_SENSOR = "douglaszuqueto/casa_01/cozinha/temperatura/sensor_01";

/************************* Declaração dos Prototypes **************************/

void initSerial();
void initWiFi();
void initMQTT();
void initMCP9808();
void readTemperature();
void sendTemperature();

/************************* Instanciação dos objetos  **************************/

Adafruit_MCP9808 mcp9808 = Adafruit_MCP9808();
WiFiClient client;
PubSubClient mqtt(client);

/********************************** Sketch ************************************/

void setup() {
  initSerial();
  initWiFi();
  initMQTT();
  initMCP9808();
}

void loop() {
  if (!mqtt.connected()) {
    reconnectMQTT();
  }

  yield();

  recconectWiFi();

  yield();

  mqtt.loop();

  yield();

  sensorLoop();

}

/*********************** Implementação dos Prototypes *************************/

void readTemperature()
{
  mcp9808.shutdown_wake(0);
  float c = mcp9808.readTempC();
  dtostrf(c, 2, 2, temp);
  delay(250);
  mcp9808.shutdown_wake(1);
}

void sendTemperature()
{
  mqtt.publish(MQTT_TOPIC_SENSOR, temp);
  Serial.println("[SENSOR] Temperatura: " + String(temp));
}

/* Função responsável por publicar a cada X segundos o valor do sensor */
void sensorLoop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > 5000 && mqtt.connected()) {
    previousMillis = currentMillis;

    readTemperature();
    sendTemperature();
  }
}

/* Conexao Serial */
void initSerial() {
  Serial.begin(115200);
}

/* Configuração da conexão WiFi */
void initWiFi() {
  delay(10);
  Serial.print("[WIFI] Conectando-se em " + String(WIFI_SSID));

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("[WIFI] SSID: " + String(WIFI_SSID));
  Serial.print("[WIFI] IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("[WIFI] Mac: ");
  Serial.println(WiFi.macAddress());
  Serial.println("");
}

/* Configuração da conexão MQTT */
void initMQTT() {
  mqtt.setServer(BROKER_MQTT, BROKER_PORT);
}

/* Inicialização do Sensor */
void initMCP9808() {
  if (!mcp9808.begin()) {
    Serial.println("[SENSOR] MCP9808 não pode ser iniciado!");
    while (1);
  }
}

/* Demais implementações */

void reconnectMQTT() {
  if (!mqtt.connected()) {
    Serial.println("[BROKER] Tentando se conectar ao Broker MQTT: " + String(BROKER_MQTT));

    while (!mqtt.connected()) {
      if (mqtt.connect("sensor_01")) {
        Serial.println("");
        Serial.println("[BROKER] Conectado");
      } else {
        Serial.print(".");
        delay(500);
      }
    }
  }
  
  Serial.println("");
}

void recconectWiFi() {
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}
