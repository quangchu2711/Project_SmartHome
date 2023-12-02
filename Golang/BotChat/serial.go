 package main
import (
    "fmt"
    "log"
    "bufio"
    "github.com/jacobsa/go-serial/serial"
    "github.com/ghodss/yaml"
    "io"
    "io/ioutil"
    mqtt "github.com/eclipse/paho.mqtt.golang"
)

type APP_FileConfig_t struct {
    /* MQTT */
    Broker string
    User string
    Password string
    TopicTxLed string
    TopicRxSensor string
}
var g_cfgFile APP_FileConfig_t
var g_ledClient mqtt.Client
var g_portDevice io.ReadWriteCloser
var g_err error
const (
   DHT11    int = 1
   LIGHT    int = 2
)

var APP_RecevieMQTTMessage mqtt.MessageHandler = func(client mqtt.Client, msg mqtt.Message) {
    mqttMsg := string(msg.Payload())
    fmt.Printf("Received message: %s from topic: %s\n", mqttMsg, msg.Topic())
    APP_SendToSerial(mqttMsg)
}

func APP_SendMQTTMessage(msg string) {
    checksum := 0
    idx := 0
    for i := 0; i < len(msg); i++ {
        fmt.Printf("0x%x ", msg[i])
    }
    if msg[0] == '[' && msg[len(msg) - 1] == ']' {
        for idx = 3; idx < int((msg[2] + 3)); idx++ {
            checksum ^= int(msg[idx])
        }
        if checksum == int(msg[idx]) {
            fmt.Println("=> Sucess")
            if int(msg[1]) == DHT11 {
                g_ledClient.Publish("serial/dht11/value", 0, false, msg)
                fmt.Println("Publish: " + "serial/dht11/value")
            } else if int(msg[1]) == LIGHT {
                g_ledClient.Publish("serial/light/value", 0, false, msg)
                fmt.Println("Publish: " + "serial/light/value")
            }
        }
    }
    fmt.Println()
    // g_ledClient.Publish(g_cfgFile.TopicRxSensor, 0, false, msg)
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

func APP_SerialBegin(port string, baud uint) (io.ReadWriteCloser, error){
    var options serial.OpenOptions

    // Set up options.
    options = serial.OpenOptions{
      PortName: port,
      BaudRate: baud,
      DataBits: 8,
      StopBits: 1,
      MinimumReadSize: 4,
    }

    //Open the port.
    var portDev io.ReadWriteCloser
    var err error
    portDev, err = serial.Open(options)

    return portDev, err
}

func APP_SendToSerial(msg string) {
    msg += "\n"
    g_portDevice.Write([]byte(msg))
    fmt.Println("Serial => " +  msg)
}

func APP_Init() {
    APP_ConfigFile()
    g_ledClient = APP_MqttBegin(g_cfgFile.Broker, g_cfgFile.User, g_cfgFile.Password, &APP_RecevieMQTTMessage)
    g_ledClient.Subscribe(g_cfgFile.TopicTxLed, 1, nil)
    fmt.Println("MQTT connected")
    APP_SendMQTTMessage("0/0/0")
    g_portDevice, g_err = APP_SerialBegin("COM4", 115200)
    if g_err != nil {
      log.Fatalf("serial.Open: %v", g_err)
    }else {
        fmt.Println("Serial connected")
    }
}


func main() {
    APP_Init()

    scanner := bufio.NewScanner(g_portDevice)
    for scanner.Scan() {
        APP_SendMQTTMessage(scanner.Text())
        // fmt.Println("Received:" + scanner.Text())
    }
}

