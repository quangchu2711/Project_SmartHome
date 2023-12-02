#ifndef _APP_SERVO_H_
#define _APP_SERVO_H_

typedef enum
{
    SERVO_ControlDoor = 0x00U,
    SERVO_Autofeed = 0x01U
}AppServo_Select_t;

void AppServo_Init(void);
void AppServo_Control(AppServo_Select_t servoSelect, uint8_t pos);

#endif
