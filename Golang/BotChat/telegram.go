package main

import (
    // "fmt"
    "log"
    "strings"
    tgbotapi "github.com/go-telegram-bot-api/telegram-bot-api/v5"
)

var g_tgBot *tgbotapi.BotAPI
var g_tgUpdates tgbotapi.UpdatesChannel

func TELEGRAM_SendNormalMessgae (groupID int64, message string) {
    newMsg := tgbotapi.NewMessage(groupID, message)
    newMsg.ParseMode = "HTML"
    g_tgBot.Send(newMsg)
}

func TELEGRAM_SendReplyMessgae (groupID int64, userName string, replyMsg string) {
    var newMsg string
    newMsg = strings.Replace("<b>userName</b> >> <i>replyMsg</i>", "userName", userName, 1)
    newMsg = strings.Replace(newMsg, "replyMsg", replyMsg, 1)

    message := tgbotapi.NewMessage(groupID, newMsg)
    message.ParseMode = "HTML"
    g_tgBot.Send(message)
}

func TELEGRAM_SendInlineButtonMsg(groupID int64, msg string) {
    var inlineCmd  tgbotapi.InlineKeyboardButton
    var rowInlineCmd []tgbotapi.InlineKeyboardButton
    var keyboard [][]tgbotapi.InlineKeyboardButton

    msgLen := len(msg)
    if msg[0] == '[' && msg[msgLen - 1] == ']' {
        inlineMsg := msg[1:(msgLen-1)]
        splitInlineMsg := strings.Split(inlineMsg, "/")
        textMsg := splitInlineMsg[0]
        cmdMsg := splitInlineMsg[1:len(splitInlineMsg)]
        /*  */
        messageSendTele := tgbotapi.NewMessage(groupID, textMsg)
        for _, text := range cmdMsg {
            inlineCmd.Text = text
            cmd := text
            inlineCmd.CallbackData = &cmd
            rowInlineCmd = append(rowInlineCmd, inlineCmd)
        }
        jump := 2
        if len(cmdMsg) % 2  != 0 {
            jump = 1
        }
        for i := 0; i < len(rowInlineCmd); i+=jump {
            sliceRowInlineCmd := rowInlineCmd[i:(i+jump)]
            keyboard = append(keyboard, sliceRowInlineCmd)
        }
        messageSendTele.ReplyMarkup = tgbotapi.InlineKeyboardMarkup{
            InlineKeyboard: keyboard,
        }

        g_tgBot.Send(messageSendTele)
    }
}

func TELEGRAM_HandleMessage(groupID string, ) {

}

func TELEGRAM_UpdateMessage(botToken string, groupID int64, homeStatus *APP_HomeStatus_t) {
    var checkMsg bool
    var userName string
    var resMsg string

    /* Config bot token */
    g_tgBot, _ = tgbotapi.NewBotAPI(botToken)
    log.Printf("Authorized on account %s", g_tgBot.Self.UserName)
    /* Update message */
    newUpdate := tgbotapi.NewUpdate(0)
    g_tgUpdates = g_tgBot.GetUpdatesChan(newUpdate)

    for update := range g_tgUpdates {
        if update.Message != nil {
            if  update.Message.NewChatMembers != nil {
                newChatMember :=  update.Message.NewChatMembers[0]
                TELEGRAM_SendReplyMessgae(update.Message.Chat.ID, "Bot", "Welcome " + newChatMember.FirstName + newChatMember.LastName + " to the group")
            }else if update.Message.LeftChatMember != nil{
                leftChatMember := update.Message.LeftChatMember
                TELEGRAM_SendReplyMessgae(update.Message.Chat.ID, "Bot", leftChatMember.FirstName + leftChatMember.LastName + " has left the group")
            }else {
                groupID = update.Message.Chat.ID
                userName = update.Message.From.FirstName + " " + update.Message.From.LastName
                if groupID == groupID {
                    TELEGRAM_SendReplyMessgae(groupID, userName, update.Message.Text)
                    checkMsg, resMsg = BOT_Progress(update.Message.Text, homeStatus)
                    if checkMsg == true {
                        TELEGRAM_SendInlineButtonMsg(groupID, resMsg)
                    }else {
                        TELEGRAM_SendNormalMessgae(groupID, resMsg)
                    }
                }
            }
        }else if update.CallbackQuery != nil {
            groupID = update.CallbackQuery.Message.Chat.ID
            userName = update.CallbackQuery.From.FirstName + " " + update.CallbackQuery.From.LastName
            if groupID == groupID {
                callback := tgbotapi.NewCallback(update.CallbackQuery.ID, update.CallbackQuery.Data)
                if _, err := g_tgBot.Request(callback); err != nil {
                    panic(err)
                }
                TELEGRAM_SendReplyMessgae(groupID, userName, update.CallbackQuery.Data)
                checkMsg, resMsg = BOT_Progress(update.CallbackQuery.Data, homeStatus)
                if checkMsg == true {
                    TELEGRAM_SendInlineButtonMsg(groupID, resMsg)
                }else {
                    TELEGRAM_SendNormalMessgae(groupID, resMsg)
                }
            }
        }
    }
}


