package main

import (
    "time"
    "strconv"
    "fmt"
)

func BOT_CreateTextAboutTimeDifference(updateTime string) string {
    var text string

    previousTime, _ := time.Parse("01-02-2006 15:04:05", updateTime)
    timeNow := time.Now()
    timeNowFormat := timeNow.Format("01-02-2006 15:04:05")
    currentTime, _ := time.Parse("01-02-2006 15:04:05", timeNowFormat)
    diffTime := currentTime.Sub(previousTime)
    hours := int(diffTime.Hours())
    minutes := int(diffTime.Minutes())
    seconds := int(diffTime.Seconds())

    if hours > 0 {
        if hours > 24 {
            text = ""
        }else {
            text = "<i>" + "Cập nhật " + strconv.Itoa(hours) + " giờ trước" + "</i>"
        }

    }else if minutes > 0 {
        text =  "<i>" + "Cập nhật " + strconv.Itoa(minutes) + " phút trước" + "</i>"

    }else if seconds > 0 {
        text =  "<i>" + "Cập nhật " + strconv.Itoa(seconds) + " giây trước" + "</i>"
    }

    return text

}

func BOT_Progress (msg string, homeStatus *APP_HomeStatus_t) (bool, string) {
    var inlineType bool
    fmt.Println("Bot: " + msg)

    switch (msg) {
    case "Help", "help":
        msg = "[Bảng điều khiển/Điều khiển thiết bị/Xem tình trạng nông trại]"
        inlineType = true;
    case "Điều khiển thiết bị", "Contorl", "control":
        msg = "[Chọn câu lệnh điều khiển/Bật đèn nông trại/Tắt đèn nông trại/Bật quạt nông trại/Tắt quạt nông trại]"
        inlineType = true;
    case "Xem tình trạng nông trại", "Status", "status":
        msg = "[Tình trạng nông trại/Trong phòng/Ngoài trời]"
        inlineType = true;
    case "Trong phòng":
        var timeDifferent string
        var lrledSta string
        lrSta := homeStatus.LivingRoom
        tem := "Nhiệt độ trong nông trại: " + "<b>" + lrSta.SensorStatus.DHT11.Temperature + "°C" + "</b>"
        hum := "Độ ẩm trong nông trại: " + "<b>" + lrSta.SensorStatus.DHT11.Humidity + "%" + "</b>"
        light := "Ánh sáng trong nông trại: " + "<b>" + lrSta.SensorStatus.LDR.Light + "</b>"
        sensorResponse := tem + "\n" + hum + "\n" + light
        fmt.Println(sensorResponse)
        timeDifferent = BOT_CreateTextAboutTimeDifference(lrSta.SensorStatus.DHT11.TimeUpdate)
        if lrSta.LedStatus == true{
            lrledSta = "Quạt nông trại" + "<b>" + " đang bật" + "</b>"
        } else {
            lrledSta = "Quạt nông trại" + "<b>" + " đang tắt" + "</b>"
        }
        if timeDifferent != "" {
            msg = sensorResponse + "\n" + timeDifferent + "\n" + lrledSta
        } else {
            msg = sensorResponse + "\n" + lrledSta
        }
        inlineType = false
    case "Ngoài trời":
        var timeDifferent string
        var kLedSta string
        kSta := homeStatus.Kitchen
        tem := "Nhiệt độ ngoài trời: " + "<b>" + kSta.SensorStatus.DHT11.Temperature + "°C" + "</b>"
        hum := "Độ ẩm ngoài trời: " + "<b>" + kSta.SensorStatus.DHT11.Humidity + "%" + "</b>"
        light := "Lượng mưa ngoài trời: " + "<b>" + kSta.SensorStatus.LDR.Light + "</b>"
        sensorResponse := tem + "\n" + hum + "\n" + light
        fmt.Println(sensorResponse)
        timeDifferent = BOT_CreateTextAboutTimeDifference(kSta.SensorStatus.LDR.TimeUpdate)
        if kSta.LedStatus == true{
            kLedSta = "Đèn nông trại" + "<b>" + " đang bật" + "</b>"
        } else {
            kLedSta = "Đèn nông trại" + "<b>" + " đang tắt" + "</b>"
        }
        if timeDifferent != "" {
            msg = sensorResponse + "\n" + timeDifferent + "\n" + kLedSta
        } else {
            msg = sensorResponse + "\n" + kLedSta
        }
        inlineType = false
    case "Bật quạt nông trại":
        APP_SendMQTTMessage("ON1")
        msg = "Quạt nông trại đã bật"
        homeStatus.Kitchen.LedStatus = true
        inlineType = false
    case "Tắt quạt nông trại":
        APP_SendMQTTMessage("OFF1")
        msg = "Quạt nông trại đã tắt"
        homeStatus.Kitchen.LedStatus = false
        inlineType = false
    case "Bật đèn nông trại":
        APP_SendMQTTMessage("ON2")
        msg = "Đèn nông trại đã bật"
        homeStatus.LivingRoom.LedStatus = true
        inlineType = false
    case "Tắt đèn nông trại":
        APP_SendMQTTMessage("OFF2")
        msg = "Đèn nông trại đã tắt"
        homeStatus.LivingRoom.LedStatus = false
        inlineType = false
    default: /* Khác tất cả tin nhắn */
        msg = "[Nông trại không hỗ trợ câu lệnh, vui lòng thử lại một số lệnh sau:\n[1] help\n[2] control\n[3] status/help/control/status]"
        inlineType = true
    }

    return inlineType, msg
}