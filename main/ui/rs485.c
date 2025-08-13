#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "can.h"
#include "pwm.h"
#include "rs485.h"
#include "ui.h"
#include "ui_helpers.h"
#include "wifi.h"

// SCREEN: ui_Main_RS485
void ui_Main_RS485_screen_init(void);
lv_obj_t* ui_Main_RS485;
void ui_event_RS485_Button1(lv_event_t* e);
lv_obj_t* ui_RS485_Button1;
lv_obj_t* ui_RS485_Label1;
lv_obj_t* ui_RS485_Keyboard1;
lv_obj_t* ui_RS485_Send;
lv_obj_t* ui_RS485_Send_Box;
void ui_event_RS485_Send_Area(lv_event_t* e);
lv_obj_t* ui_RS485_Send_Area;
void ui_event_RS485_Button3(lv_event_t* e);
lv_obj_t* ui_RS485_Button3;
lv_obj_t* ui_RS485_Label3;
void ui_event_RS485_Button4(lv_event_t* e);
lv_obj_t* ui_RS485_Button4;
lv_obj_t* ui_RS485_Label4;
lv_obj_t* ui_RS485_Read;
lv_obj_t* ui_RS485_Read_Area;
lv_obj_t* ui_RS485_Read_Box;
void ui_event_RS485_Button2(lv_event_t* e);
lv_obj_t* ui_RS485_Button2;
lv_obj_t* ui_RS485_Label2;
lv_obj_t* ui_RS485_Roller1;
// CUSTOM VARIABLES
bool RS485_Clear = false;                     // Flag for RS485 clear action
uint32_t RS485_BaudRate = ECHO_TEST_BAUDRATE; // Baud rate for RS485 communication

void ui_event_RS485(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  // Triggered when the event is a click
  if (event_code == LV_EVENT_CLICKED) {
    // Change screen to the RS485 screen and start the RS485 task
    _ui_screen_change(&ui_Main_RS485, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, &ui_Main_RS485_screen_init);
    xTaskCreate(rs485_task, "rs485_task", 3 * 1024, NULL, 2, &rs485_TaskHandle);
  }
}

// Event handler for stopping RS485 and deleting the RS485 task
void ui_event_RS485_Button1(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  // Triggered when the event is a click
  if (event_code == LV_EVENT_CLICKED) {
    // Change screen back to the main screen and close RS485
    _ui_screen_change(&ui_Main, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, &ui_Main_screen_init);
    DEV_UART_Delete();             // Delete UART for RS485 communication
    vTaskDelete(rs485_TaskHandle); // Delete the RS485 task
    rs485_TaskHandle = NULL;       // Reset the task handle
  }
}

// Event handler for the RS485 send area (keyboard interaction)
void ui_event_RS485_Send_Area(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  // Triggered when the send area gains focus
  if (event_code == LV_EVENT_FOCUSED) {
    // Remove the hidden flag from the keyboard and set the keyboard target to the send area
    _ui_flag_modify(ui_RS485_Keyboard1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    _ui_keyboard_set_target(ui_RS485_Keyboard1, ui_RS485_Send_Area);

    lv_textarea_set_placeholder_text(ui_Create_Uesr_Name, "");
    lv_obj_set_style_bg_opa(ui_Create_Uesr_Name, 255, LV_PART_CURSOR | LV_STATE_DEFAULT);
  }
  // Triggered when the send area loses focus
  if (event_code == LV_EVENT_DEFOCUSED) {
    _ui_flag_modify(ui_RS485_Keyboard1, LV_OBJ_FLAG_HIDDEN,
                    _UI_MODIFY_FLAG_ADD); // Add the hidden flag back to keyboard

    lv_textarea_set_placeholder_text(ui_Create_Uesr_Name, "Send...");
    lv_obj_set_style_bg_opa(ui_Create_Uesr_Name, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);
  }
  // Triggered when the send area is ready to send data
  if (event_code == LV_EVENT_READY) {
    RS485Send(e); // Call the function to send RS485 data

    lv_textarea_set_placeholder_text(ui_Create_Uesr_Name, "Send...");
    lv_obj_set_style_bg_opa(ui_Create_Uesr_Name, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);
  }
}

// Event handler for RS485 baud rate settings button (toggle visibility)
void ui_event_RS485_Button3(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  // Triggered when the button is pressed
  if (event_code == LV_EVENT_PRESSED) {
    // Toggle visibility of baud rate roller and save button
    _ui_flag_modify(ui_RS485_Roller1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_TOGGLE);
    _ui_flag_modify(ui_RS485_Button4, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_TOGGLE);
  }
}

// Event handler to save the selected RS485 baud rate
void ui_event_RS485_Button4(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  // Triggered when the button is clicked
  if (event_code == LV_EVENT_CLICKED) {
    RS485Savebaud(e);                                                           // Save the selected baud rate
    _ui_flag_modify(ui_RS485_Roller1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD); // Hide the baud rate roller
    _ui_flag_modify(ui_RS485_Button4, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD); // Hide the save button
  }
}

// Event handler to clear RS485 received data
void ui_event_RS485_Button2(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  // Triggered when the button is clicked
  if (event_code == LV_EVENT_CLICKED) {
    RS485Clear(e); // Clear the received RS485 data
  }
}
