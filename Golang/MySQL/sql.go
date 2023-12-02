package main
import (
    "time"
    "database/sql"
    _ "github.com/go-sql-driver/mysql"
    "log"
    "fmt"
    mqtt "github.com/eclipse/paho.mqtt.golang"
    "github.com/ghodss/yaml"
    "io/ioutil"
    "strconv"
    "strings"
)

type Mqtt struct {
    Broker string
    TopicSensor string
}

type MySql struct {
    User string
    Password string
    Database string
    Table string
}

type FileConfig struct {
    MqttConfig Mqtt
    MySqlConfig MySql
}
var cfg FileConfig
var mqttClientSensor mqtt.Client
var db *sql.DB
var err error

var messageSensorHandler mqtt.MessageHandler = func(client mqtt.Client, msg mqtt.Message) {
    topicMsg := string(msg.Payload())
    sensorValue := strings.Split(topicMsg, "/")
    fmt.Println(sensorValue)
    if len(sensorValue) == 6 {
        temvalue1,_ := strconv.Atoi(sensorValue[0])
        humValue1,_ := strconv.Atoi(sensorValue[1])
        temvalue2,_ := strconv.Atoi(sensorValue[2])
        humValue2,_ := strconv.Atoi(sensorValue[3])
        lightValue,_ := strconv.Atoi(sensorValue[4])
        rainValue,_ := strconv.Atoi(sensorValue[5])
        timeValue := time.Now()
        fmt.Printf("Received message: [%s] from topic: %s\n", topicMsg, msg.Topic())
        _, err = db.Exec("INSERT INTO" + " " + cfg.MySqlConfig.Table + "(Temperature1, Humidity1, Temperature2, Humidity2, Light, Rain, Time) VALUES (?, ?, ?, ?, ?, ?, ?)",
                                                    temvalue1, humValue1, temvalue2, humValue2,lightValue, rainValue, timeValue)
        if err != nil {
            panic(err)
        }
        fmt.Printf("Saved database")
    }
}

func mqttBegin(broker string, messagePubHandler *mqtt.MessageHandler) mqtt.Client {

    var opts *mqtt.ClientOptions = new(mqtt.ClientOptions)

    opts = mqtt.NewClientOptions()
    opts.AddBroker(broker)

    opts.SetDefaultPublishHandler(*messagePubHandler)

    client := mqtt.NewClient(opts)
    if token := client.Connect(); token.Wait() && token.Error() != nil {
        panic(token.Error())
    }
    return client
}

func Handle_YamlFile() {
    yfile, err := ioutil.ReadFile("config.yaml")

    if err != nil {

      log.Fatal(err)
    }

    err2 := yaml.Unmarshal(yfile, &cfg)

    if err2 != nil {

      log.Fatal(err2)
    }

}

func main() {
    fmt.Println("....")
    Handle_YamlFile()

    mqttClientSensor = mqttBegin(cfg.MqttConfig.Broker, &messageSensorHandler)
    mqttClientSensor.Subscribe(cfg.MqttConfig.TopicSensor, 1, nil)
    addrString := cfg.MySqlConfig.User + ":" + cfg.MySqlConfig.Password + "@tcp(127.0.0.1:3306)/" + cfg.MySqlConfig.Database
    db, err = sql.Open("mysql", addrString)
    if err != nil {
        panic(err)
    } else if err = db.Ping(); err != nil {
        panic(err)
    }
    defer db.Close()

    _, err = db.Exec(`CREATE TABLE IF NOT EXISTS` + ` ` + cfg.MySqlConfig.Table + `(
    ID INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    Temperature1 INT,
    Humidity1 INT,
    Temperature2 INT,
    Humidity2 INT,
    Light INT,
    Rain INT,
    Time DATETIME)`)
    if err != nil {
        panic(err)
    }

    fmt.Println("Connected")

    for {

        time.Sleep(2 * time.Second)
    }
}