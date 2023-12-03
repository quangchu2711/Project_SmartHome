#include "WiFi.h"
#include <PubSubClient.h>

#define DASHBOARD_MQTT_SERVER               "broker.hivemq.com"

#define DASHBOARD_MQTT_PORT                 1883
#define DASHBOARD_MQTT_USER                 "test"
#define DASHBOARD_MQTT_PASSWORD             "123456"

#define DASHBOARD_TOPIC_TEMPERATURE1        "MySensor/Temperature1"
#define DASHBOARD_TOPIC_HUMIDITY1           "MySensor/Humidity1"
#define DASHBOARD_TOPIC_TEMPERATURE2        "MySensor/Temperature2"
#define DASHBOARD_TOPIC_HUMIDITY2           "MySensor/Humidity2"
#define DASHBOARD_TOPIC_LIGHT               "MySensor/Light"
#define DASHBOARD_TOPIC_RAIN                "MySensor/Rain"
#define DASHBOARD_TOPIC_SOIL                "MySensor/Soil"

#define DASHBOARD_TOPIC_LED                 "MyLed/Control"
#define DASHBOARD_TOPIC_THRESHOLD_RAIN      "MySensor/Threshold/Rain"
#define DASHBOARD_TOPIC_THRESHOLD_LIGHT     "MySensor/Threshold/Light"

#define DASHBOARD_TOPIC_MODE                "MyRelay/Mode"
#define DASHBOARD_TOPIC_RELAY               "MyRelay/Control"
#define DASHBOARD_TOPIC_TEST                "MyTest/Check"

#define DASHBOARD_TOPIC_SENSOR              "MySensor/Value"

#define CHATBOT_TOPIC_CONTROL               "farm/device/control"
#define CHATBOT_TOPIC_DHT                   "farm/DHT11/control"
#define CHATBOT_TOPIC_ADC                   "farm/ADC/value"
#define CHATBOT_TOPIC_STATUS                "farm/device/status"

#define DASHBOARD_BUFFER_LENGTH 9
#define MESSAGE_START                       ('(')
#define MESSAGE_END                         (')')

WiFiClient wifiClient;
PubSubClient client(wifiClient);

bool g_receiveFlag = false;
uint8_t g_readData;
uint8_t g_bufferRx[DASHBOARD_BUFFER_LENGTH];
uint8_t g_idx = 0;

const char* g_ssid = "Quang";
const char* g_password = "12233445";
int g_updateTime = 0;
uint8_t g_ledSta = 0;

void setup(void)
{
    Serial.begin(115200);
    Serial.setTimeout(500);
    pinMode(2, OUTPUT);

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

void loop()
{
    client.loop();
    if (!client.connected()) {
        DashBoard_ConnectToBroker();
    }
    Dashboard_ReceiveDataFromSTM32();
}

void Dashboard_ReceiveDataFromSTM32(void)
{
    if (Serial.available() > 0)
    {
        g_readData = Serial.read();
        if (g_readData == MESSAGE_START && g_receiveFlag == false)
        {
            g_receiveFlag = true;
            g_idx = 0;
            g_bufferRx[DASHBOARD_BUFFER_LENGTH - 1] = '\0';
        }
        else
        {
            if (g_receiveFlag == true)
            {
                if (g_idx < DASHBOARD_BUFFER_LENGTH)
                {
                    g_bufferRx[g_idx++] = g_readData;
                }
                else if (g_readData == MESSAGE_END || g_idx == DASHBOARD_BUFFER_LENGTH)
                {
                    g_bufferRx[DASHBOARD_BUFFER_LENGTH - 1] = '\0';
                    Dashboard_HandleDataFromSTM32();
                    g_receiveFlag = false;
                    g_idx = 0;
                }
            }
        }
    }
}
uint8_t checksumVal = 0;
char tem1[4];
char hum1[4];
char tem2[4];
char hum2[4];
char light[4];
char rain[4];
char sensor[20];
char dhtSensor[20];
char adcSensor[20];
void Dashboard_HandleDataFromSTM32(void)
{

    checksumVal = g_bufferRx[0] ^ g_bufferRx[1] ^ g_bufferRx[2] ^ g_bufferRx[3] ^ g_bufferRx[4] ^ g_bufferRx[5];
    if (checksumVal == g_bufferRx[6])
    {
        sprintf(tem1, "%d", g_bufferRx[0]);
        client.publish(DASHBOARD_TOPIC_TEMPERATURE1, tem1, true);
        sprintf(hum1, "%d", g_bufferRx[1]);
        client.publish(DASHBOARD_TOPIC_HUMIDITY1, hum1, true);

        sprintf(tem2, "%d", g_bufferRx[2]);
        client.publish(DASHBOARD_TOPIC_TEMPERATURE2, tem2, true);

        sprintf(hum2, "%d", g_bufferRx[3]);
        client.publish(DASHBOARD_TOPIC_HUMIDITY2, hum2, true);

        sprintf(light, "%d", g_bufferRx[4]);
        client.publish(DASHBOARD_TOPIC_LIGHT, light, true);

        sprintf(rain, "%d", g_bufferRx[5]);
        client.publish(DASHBOARD_TOPIC_RAIN, rain, true);

        /* MySQL */
        sprintf(sensor, "%d/%d/%d/%d/%d/%d", g_bufferRx[0], g_bufferRx[1], g_bufferRx[2], g_bufferRx[3], g_bufferRx[4], g_bufferRx[5]);
        client.publish(DASHBOARD_TOPIC_SENSOR, sensor, true);

        /* Chat bot */
        sprintf(dhtSensor, "%d/%d/%d/%d", g_bufferRx[0], g_bufferRx[1], g_bufferRx[2], g_bufferRx[3]);
        client.publish(CHATBOT_TOPIC_DHT, dhtSensor, true);
        sprintf(adcSensor, "%d/%d", g_bufferRx[4], g_bufferRx[5]);
        client.publish(CHATBOT_TOPIC_ADC, adcSensor, true);
    }
}

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

bool g_mode = false;
void Dashboard_Callback(char* topic, byte *payload, unsigned int length) {
    char statusMsg[length + 1];
    memcpy(statusMsg, payload, length);
    statusMsg[length] = NULL;
    String topicMsg(statusMsg);

    if (String(topic) == CHATBOT_TOPIC_CONTROL)
    {
        if (topicMsg == "ON1")
        {
            Serial.println("D1");
        }
        else if (topicMsg == "OFF1")
        {
            Serial.println("D0");
        }
        else if (topicMsg == "ON2")
        {
            Serial.println("F1");
        }
        else if (topicMsg == "OFF2")
        {
            Serial.println("F0");
        }
        client.publish(CHATBOT_TOPIC_STATUS, statusMsg, true);
    }
    else if (String(topic) == DASHBOARD_TOPIC_THRESHOLD_RAIN)
    {
        Serial.println("R" +topicMsg);
    }
    else if (String(topic) == DASHBOARD_TOPIC_THRESHOLD_LIGHT)
    {
        Serial.println("L" + topicMsg);
    }
    // else if (String(topic) == DASHBOARD_TOPIC_MODE)
    // {
    //     if (topicMsg == "true")
    //     {
    //         g_mode = true;
    //         Serial.println("M1");
    //     }
    //     else if (topicMsg == "false")
    //     {
    //         g_mode = false;
    //         Serial.println("M0");
    //     }
    // }
    // else if (String(topic) == DASHBOARD_TOPIC_RELAY)
    // {
    //     if (g_mode == true)
    //     {
    //       if (topicMsg == "on")
    //       {
    //           Serial.println("E1");
    //       }
    //       else if (topicMsg == "off")
    //       {
    //           Serial.println("E0");
    //       }
    //     }
    // }
}
