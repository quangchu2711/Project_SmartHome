package main

import (
    "time"
    "crypto/aes"
    "crypto/cipher"
    "crypto/rand"
    "fmt"
    "io"
    mqtt "github.com/eclipse/paho.mqtt.golang"
)

var mqttClient mqtt.Client
func mqttBegin(broker string, user string, pw string) mqtt.Client {
    var opts *mqtt.ClientOptions = new(mqtt.ClientOptions)

    opts = mqtt.NewClientOptions()
    opts.AddBroker(fmt.Sprintf(broker))
    opts.SetUsername(user)
    opts.SetPassword(pw)
    client := mqtt.NewClient(opts)
    if token := client.Connect(); token.Wait() && token.Error() != nil {
        panic(token.Error())
    }

    return client
}

func main() {
    fmt.Println("Encryption Program v0.01")

    text := []byte("Nguyen Duc Phang")
    key := []byte("passphrasewhichneedstobe32bytes!")

    // generate a new aes cipher using our 32 byte long key
    c, err := aes.NewCipher(key)
    // if there are any errors, handle them
    if err != nil {
        fmt.Println(err)
    }

    // gcm or Galois/Counter Mode, is a mode of operation
    // for symmetric key cryptographic block ciphers
    // - https://en.wikipedia.org/wiki/Galois/Counter_Mode
    gcm, err := cipher.NewGCM(c)
    // if any error generating new GCM
    // handle them
    if err != nil {
        fmt.Println(err)
    }

    // creates a new byte array the size of the nonce
    // which must be passed to Seal
    nonce := make([]byte, gcm.NonceSize())
    // populates our nonce with a cryptographically secure
    // random sequence
    if _, err = io.ReadFull(rand.Reader, nonce); err != nil {
        fmt.Println(err)
    }

    // here we encrypt our text using the Seal function
    // Seal encrypts and authenticates plaintext, authenticates the
    // additional data and appends the result to dst, returning the updated
    // slice. The nonce must be NonceSize() bytes long and unique for all
    // time, for a given key.

    // fmt.Println(gcm.Seal(nonce, nonce, text, nil))

    asmEn := gcm.Seal(nonce, nonce, text, nil)

    /*Ma hoa sau do moi publish du lieu*/
    mqttClient = mqttBegin("localhost:1883", "Phan", "2000")
    for {
        mqttClient.Publish("test/encrypt", 0, false, asmEn)
        fmt.Print("publish: " )
        fmt.Println(asmEn)
        fmt.Println("==================================================")
        time.Sleep(2 * time.Second)
    }
}