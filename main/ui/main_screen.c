#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "can.h"
#include "pwm.h"
#include "rs485.h"
#include "ui.h"
#include "ui_helpers.h"
#include "wifi.h"

// SCREEN: ui_Main
void ui_Main_screen_init(void);
lv_obj_t* ui_Main;
void ui_event_Wifi(lv_event_t* e);
lv_obj_t* ui_Wifi;
lv_obj_t* ui_Wifi_Label0;
void ui_event_RS485(lv_event_t* e);
lv_obj_t* ui_RS485;
lv_obj_t* ui_RS485_Label0;
void ui_event_CAN(lv_event_t* e);
lv_obj_t* ui_CAN;
lv_obj_t* ui_CAN_Label0;
void ui_event_PWM(lv_event_t* e);
lv_obj_t* ui_PWM;
lv_obj_t* ui_PWM_Label10;
