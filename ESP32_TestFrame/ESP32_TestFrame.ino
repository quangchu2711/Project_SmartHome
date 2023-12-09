#include <stdint.h>
#include "WiFi.h"
#include <PubSubClient.h>

#define DASHBOARD_BUFFER_LENGTH 30
#define MESSAGE_START                       ('(')
#define MESSAGE_END                         (')')

#define DASHBOARD_MQTT_SERVER               "broker.hivemq.com"
#define DASHBOARD_MQTT_PORT                  1883
#define DASHBOARD_MQTT_USER                 "test"
#define DASHBOARD_MQTT_PASSWORD             "123456"

WiFiClient wifiClient;
PubSubClient client(wifiClient);

char g_readData;
char g_bufferRx[DASHBOARD_BUFFER_LENGTH];
bool g_receiveFlag;
uint8_t g_frameIdx = 0;

char* g_ssid = "Quang";
char* g_password = "12233445";

void DashBoard_ConnectToBroker() {
    while (!client.connected()) {
        String clientId = "ESP32";
        clientId += String(random(0xffff), HEX);
        if (client.connect(clientId.c_str(), DASHBOARD_MQTT_USER, DASHBOARD_MQTT_PASSWORD)) {
            client.subscribe(DASHBOARD_TOPIC_LED);
            client.subscribe(DASHBOARD_TOPIC_THRESHOLD_RAIN);
            client.subscribe(DASHBOARD_TOPIC_THRESHOLD_LIGHT);
            client.subscribe(DASHBOARD_TOPIC_MODE);
            client.subscribe(DASHBOARD_TOPIC_RELAY);
            client.subscribe(CHATBOT_TOPIC_CONTROL);
            digitalWrite(2, LOW);
        } else {
            g_ledSta = !g_ledSta;
            digitalWrite(2, g_ledSta);
            delay(1000);
        }
    }
}

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);

    pinMode(2, OUTPUT);

    g_frameIdx = 0;
    g_receiveFlag = false;

    // Connect to WiFi
    WiFi.begin(g_ssid, g_password);
    while (WiFi.status() != WL_CONNECTED)
    {
        g_ledSta = !g_ledSta;
        digitalWrite(2, g_ledSta);
        delay(100);
    }
    digitalWrite(2, 0);

    client.setServer(DASHBOARD_MQTT_SERVER, DASHBOARD_MQTT_PORT);
    client.setCallback(Dashboard_Callback);

    DashBoard_ConnectToBroker();
}

void loop() {
    // put your main code here, to run repeatedly:
    g_readData = Serial.read();
    if (g_readData == MESSAGE_START && g_receiveFlag == false)
    {
        g_receiveFlag = true;
        g_frameIdx = 0;
        g_bufferRx[DASHBOARD_BUFFER_LENGTH - 1] = '\0';
    }
    else
    {
        if (g_receiveFlag == true)
        {
            if (g_frameIdx < DASHBOARD_BUFFER_LENGTH)
            {
                g_bufferRx[g_frameIdx++] = g_readData;
            }
            else if (g_readData == MESSAGE_END || g_frameIdx == DASHBOARD_BUFFER_LENGTH-1)
            {
                Serial.println(g_bufferRx);

                for (g_frameIdx = 0; g_frameIdx < DASHBOARD_BUFFER_LENGTH; g_frameIdx++)
                {
                    g_bufferRx[g_frameIdx] = '\0';
                }
                g_receiveFlag = false;
                g_frameIdx = 0;
            }
        }
    }
}