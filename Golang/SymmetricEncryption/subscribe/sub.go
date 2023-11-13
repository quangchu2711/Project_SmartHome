package main

import (
    "time"
    "crypto/aes"
    "crypto/cipher"
    "fmt"
    mqtt "github.com/eclipse/paho.mqtt.golang"
)

var ciphertextTest []byte
var messagePubHandler mqtt.MessageHandler = func(client mqtt.Client, msg mqtt.Message) {
    fmt.Print("Received message: ")
    fmt.Println(msg.Payload())
    fmt.Printf("from topic: %s\n", msg.Topic())

    key := []byte("passphrasewhichneedstobe32bytes!")

    ciphertext := msg.Payload()

    c, err := aes.NewCipher(key)
    if err != nil {
        fmt.Println(err)
    }

    gcm, err := cipher.NewGCM(c)
    if err != nil {
        fmt.Println(err)
    }

    nonceSize := gcm.NonceSize()
    if len(ciphertext) < nonceSize {
        fmt.Println(err)
    }

    nonce, ciphertext := ciphertext[:nonceSize], ciphertext[nonceSize:]
    plaintext, err := gcm.Open(nil, nonce, ciphertext, nil)
    if err != nil {
        fmt.Println(err)
    }
    fmt.Println(string(plaintext))

}

var mqttClient mqtt.Client
func mqttBegin(broker string, user string, pw string, messagePubHandler *mqtt.MessageHandler) mqtt.Client {
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

func main() {
    mqttClient = mqttBegin("localhost:1883", "Phan", "2000", &messagePubHandler)
    mqttClient.Subscribe("test/encrypt", 1, nil)
    fmt.Println("Connected")

    for {

        time.Sleep(2 * time.Second)
    }
}