package main

import (
    "log"
    "fmt"
    "io/ioutil"
    "github.com/ghodss/yaml"
    "time"
    "strconv"
    mqtt "github.com/eclipse/paho.mqtt.golang"
    "strings"
    tgbotapi "github.com/go-telegram-bot-api/telegram-bot-api/v5"
)

type APP_FileConfig_t struct {
    /* Telegram */
    BotName string
    BotToken string
    GroupID int64
    /* MQTT */
    Broker string
    User string
    Password string
    TopicTxLed string
    TopicRxSensor string
    TopicDeviceSatus string
}

type APP_DHTSensor_t struct {
    Temperature string
    Humidity string
    TimeUpdate string
}

type APP_LightSensor_t struct {
    Light string
    TimeUpdate string
}

type APP_Sensor_t struct {
    DHT11 APP_DHTSensor_t
    LDR APP_LightSensor_t
}

type APP_LivingRoomStatus_t struct {
    SensorStatus APP_Sensor_t
    LedStatus bool
}

type APP_KitchenStatus_t struct {
    SensorStatus APP_Sensor_t
    LedStatus bool
}

type APP_HomeStatus_t struct {
    LivingRoom APP_LivingRoomStatus_t
    Kitchen APP_KitchenStatus_t
}

var g_cfgFile APP_FileConfig_t
var g_serialClient mqtt.Client
var g_serialClient1 mqtt.Client
var g_serialClient2 mqtt.Client
var g_home APP_HomeStatus_t
const (
   DHT11    int = 1
   ADC    int = 2
)

var APP_RecevieMQTTMessage mqtt.MessageHandler = func(client mqtt.Client, msg mqtt.Message) {
    mqttMsg := string(msg.Payload())
    fmt.Printf("Received message from topic: %s - msg: %s\n", msg.Topic(), mqttMsg)
    topic := strings.Split(msg.Topic(), "/")
    fmt.Printf("%s", topic[1])
    if topic[1] == "DHT11" {
        g_home.LivingRoom.SensorStatus.DHT11.Temperature = strconv.Itoa(int(mqttMsg[0]))
        g_home.LivingRoom.SensorStatus.DHT11.Humidity = strconv.Itoa(int(mqttMsg[1]))
        g_home.Kitchen.SensorStatus.DHT11.Temperature = strconv.Itoa(int(mqttMsg[2]))
        g_home.Kitchen.SensorStatus.DHT11.Humidity = strconv.Itoa(int(mqttMsg[3]))
        currentTime1 := time.Now()
        g_home.LivingRoom.SensorStatus.DHT11.TimeUpdate = currentTime1.Format("01-02-2006 15:04:05")
        fmt.Println("=> Updated DHT11 value")
    } else if topic[1] == "ADC" {
        g_home.LivingRoom.SensorStatus.LDR.Light = strconv.Itoa(int(mqttMsg[0]))
        g_home.Kitchen.SensorStatus.LDR.Light = strconv.Itoa(int(mqttMsg[1]))
        currentTime2 := time.Now()
        g_home.Kitchen.SensorStatus.LDR.TimeUpdate = currentTime2.Format("01-02-2006 15:04:05")
        fmt.Println("=> Updated ADC value")
    }
}

func APP_SendMQTTMessage(msg string) {
    g_serialClient.Publish(g_cfgFile.TopicTxLed, 0, false, msg)
    fmt.Println("Publish: " + g_cfgFile.TopicTxLed + " msg: " + msg)
}

func APP_ConfigFile() {
    yfile, errReadFile := ioutil.ReadFile("appConfig.yaml")
    if errReadFile != nil {
      log.Fatal(errReadFile)
    }
    errUnFile := yaml.Unmarshal(yfile, &g_cfgFile)
    if errUnFile != nil {
      log.Fatal(errUnFile)
    }
}

func APP_MqttBegin(broker string, user string, pw string, messagePubHandler *mqtt.MessageHandler) mqtt.Client {
    var opts *mqtt.ClientOptions = new(mqtt.ClientOptions)

    opts = mqtt.NewClientOptions()
    opts.AddBroker(broker)
    opts.SetUsername(user)
    opts.SetPassword(pw)
    opts.SetDefaultPublishHandler(*messagePubHandler)
    client := mqtt.NewClient(opts)
    if token := client.Connect(); token.Wait() && token.Error() != nil {
        panic(token.Error())
    }

    return client
}

func APP_Init() {
    /* Đoc thông tin file xong, để lấy một số thông tin:
    ví dụ: Bot token, tên bot, groupID */
    APP_ConfigFile()
    /* Config bot token */
    g_tgBot, _ = tgbotapi.NewBotAPI(g_cfgFile.BotToken)
    log.Printf("Authorized on account %s", g_tgBot.Self.UserName)
    /* Hiên thị thông tin lên bot token */
    fmt.Println(g_cfgFile.BotToken)
    g_serialClient = APP_MqttBegin(g_cfgFile.Broker, g_cfgFile.User, g_cfgFile.Password, &APP_RecevieMQTTMessage)
    g_serialClient.Subscribe(g_cfgFile.TopicRxSensor, 1, nil)
    g_serialClient1 = APP_MqttBegin(g_cfgFile.Broker, g_cfgFile.User, g_cfgFile.Password, &APP_RecevieStatusMsg)
    g_serialClient1.Subscribe("farm/device/status", 1, nil)
    g_serialClient2 = APP_MqttBegin(g_cfgFile.Broker, g_cfgFile.User, g_cfgFile.Password, &APP_RecevieWarningMsg)
    g_serialClient2.Subscribe("farm/warning", 1, nil)
    fmt.Println("MQTT Connected")
}

func APP_Handle() {
    TELEGRAM_UpdateMessage(g_cfgFile.GroupID, &g_home)
}

func main() {
    APP_Init()
    APP_Handle()
}
