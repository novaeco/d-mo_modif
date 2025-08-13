#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "can.h"
#include "pwm.h"
#include "rs485.h"
#include "ui.h"
#include "ui_helpers.h"
#include "wifi.h"

// SCREEN: ui_Main_CAN
void ui_Main_CAN_screen_init(void);
lv_obj_t* ui_Main_CAN;
void ui_event_CAN_Button1(lv_event_t* e);
lv_obj_t* ui_CAN_Button1;
lv_obj_t* ui_CAN_Label1;
lv_obj_t* ui_CAN_Keyboard0;
lv_obj_t* ui_CAN_Send1;
lv_obj_t* ui_CAN_Send_Box;
void ui_event_CAN_Send_ID(lv_event_t* e);
lv_obj_t* ui_CAN_Send_ID;
void ui_event_CAN_Send_Len(lv_event_t* e);
lv_obj_t* ui_CAN_Send_Len;
void ui_event_CAN_Send_Data(lv_event_t* e);
lv_obj_t* ui_CAN_Send_Data;
void ui_event_CAN_Button2(lv_event_t* e);
lv_obj_t* ui_CAN_Button2;
lv_obj_t* ui_CAN_Label2;
void ui_event_CAN_Button3(lv_event_t* e);
lv_obj_t* ui_CAN_Button3;
lv_obj_t* ui_CAN_Label3;
lv_obj_t* ui_CAN_Read1;
lv_obj_t* ui_CAN_Read_Area;
lv_obj_t* ui_CAN_Read_Box;
void ui_event_CAN_Button4(lv_event_t* e);
lv_obj_t* ui_CAN_Button4;
lv_obj_t* ui_CAN_Label4;
lv_obj_t* ui_CAN_Roller;
// CUSTOM VARIABLES
bool CAN_Clear = false; // Flag for CAN clear action

void ui_event_CAN(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  // Triggered when the event is a click
  if (event_code == LV_EVENT_CLICKED) {
    _ui_screen_change(&ui_Main_CAN, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0,
                      &ui_Main_CAN_screen_init); // Change screen to CAN screen
    // Start the CAN task
    xTaskCreate(can_task, "can_task", 3 * 1024, NULL, 2, &can_TaskHandle);
  }
}

// Event handler for stopping CAN and deleting the task
void ui_event_CAN_Button1(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  if (event_code == LV_EVENT_CLICKED) {
    _ui_screen_change(&ui_Main, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, &ui_Main_screen_init); // Change screen to main screen
    // Uninstall CAN driver and delete task
    can_uninstall();             // Uninstall CAN driver
    vTaskDelete(can_TaskHandle); // Delete the CAN task
    can_TaskHandle = NULL;       // Reset task handle
  }
}

// Event handler for CAN ID input field (keyboard interaction)
void ui_event_CAN_Send_ID(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  // Triggered when the CAN ID field gains focus
  if (event_code == LV_EVENT_FOCUSED) {
    _ui_keyboard_set_target(ui_CAN_Keyboard0, ui_CAN_Send_ID); // Set keyboard target to CAN ID field
    _ui_flag_modify(ui_CAN_Keyboard0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE); // Show keyboard

    lv_obj_set_style_bg_opa(ui_CAN_Send_Len, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);
    lv_textarea_set_placeholder_text(ui_CAN_Send_Len, "Length...");
    lv_obj_set_style_bg_opa(ui_CAN_Send_Data, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);
    lv_textarea_set_placeholder_text(ui_CAN_Send_Data, "Data(Hex)...");
    lv_obj_set_style_bg_opa(ui_CAN_Send_ID, 255, LV_PART_CURSOR | LV_STATE_DEFAULT);
    lv_textarea_set_placeholder_text(ui_CAN_Send_ID, "");
  }
  // Triggered when the CAN ID field loses focus
  if (event_code == LV_EVENT_DEFOCUSED) {
    _ui_flag_modify(ui_CAN_Keyboard0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD); // Hide keyboard
    lv_obj_set_style_bg_opa(ui_CAN_Send_ID, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);
    lv_textarea_set_placeholder_text(ui_CAN_Send_ID, "ID...");
  }
  // Triggered when the input is ready
  if (event_code == LV_EVENT_READY) {
    _ui_state_modify(ui_CAN_Send_ID, LV_STATE_FOCUSED, _UI_MODIFY_STATE_REMOVE); // Remove focus from CAN ID
    _ui_state_modify(ui_CAN_Send_Len, LV_STATE_FOCUSED, _UI_MODIFY_STATE_ADD);   // Focus on length field
    _ui_keyboard_set_target(ui_CAN_Keyboard0, ui_CAN_Send_Len);                  // Set keyboard target to length field
    lv_obj_set_style_bg_opa(ui_CAN_Send_ID, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);
    lv_textarea_set_placeholder_text(ui_CAN_Send_ID, "ID...");
    lv_obj_set_style_bg_opa(ui_CAN_Send_Len, 255, LV_PART_CURSOR | LV_STATE_DEFAULT);
    lv_textarea_set_placeholder_text(ui_CAN_Send_Len, "");
  }
}

// Event handler for CAN message length input field (keyboard interaction)
void ui_event_CAN_Send_Len(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  // Triggered when the length field gains focus
  if (event_code == LV_EVENT_FOCUSED) {
    _ui_keyboard_set_target(ui_CAN_Keyboard0, ui_CAN_Send_Len); // Set keyboard target to length field
    _ui_flag_modify(ui_CAN_Keyboard0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE); // Show keyboard

    lv_obj_set_style_bg_opa(ui_CAN_Send_ID, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);
    lv_textarea_set_placeholder_text(ui_CAN_Send_ID, "ID...");
    lv_obj_set_style_bg_opa(ui_CAN_Send_Data, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);
    lv_textarea_set_placeholder_text(ui_CAN_Send_Data, "Data(Hex)...");

    lv_obj_set_style_bg_opa(ui_CAN_Send_Len, 255, LV_PART_CURSOR | LV_STATE_DEFAULT);
    lv_textarea_set_placeholder_text(ui_CAN_Send_Len, "");
  }
  // Triggered when the length field loses focus
  if (event_code == LV_EVENT_DEFOCUSED) {
    _ui_flag_modify(ui_CAN_Keyboard0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD); // Hide keyboard

    lv_obj_set_style_bg_opa(ui_CAN_Send_Len, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);
    lv_textarea_set_placeholder_text(ui_CAN_Send_Len, "Length...");
  }
  // Triggered when the input is ready
  if (event_code == LV_EVENT_READY) {
    _ui_state_modify(ui_CAN_Send_Len, LV_STATE_FOCUSED, _UI_MODIFY_STATE_REMOVE); // Remove focus from length field
    _ui_state_modify(ui_CAN_Send_Data, LV_STATE_FOCUSED, _UI_MODIFY_STATE_ADD);   // Focus on data field
    _ui_keyboard_set_target(ui_CAN_Keyboard0, ui_CAN_Send_Data);                  // Set keyboard target to data field

    lv_obj_set_style_bg_opa(ui_CAN_Send_Len, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);
    lv_textarea_set_placeholder_text(ui_CAN_Send_Len, "Length...");
    lv_obj_set_style_bg_opa(ui_CAN_Send_Data, 255, LV_PART_CURSOR | LV_STATE_DEFAULT);
    lv_textarea_set_placeholder_text(ui_CAN_Send_Data, "");
  }
}

// Event handler for CAN message data input field (keyboard interaction)
void ui_event_CAN_Send_Data(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  // Triggered when the data field gains focus
  if (event_code == LV_EVENT_FOCUSED) {
    _ui_flag_modify(ui_CAN_Keyboard0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE); // Show keyboard
    _ui_keyboard_set_target(ui_CAN_Keyboard0, ui_CAN_Send_Data);                   // Set keyboard target to data field

    lv_obj_set_style_bg_opa(ui_CAN_Send_ID, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);
    lv_textarea_set_placeholder_text(ui_CAN_Send_ID, "ID...");
    lv_obj_set_style_bg_opa(ui_CAN_Send_Len, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);
    lv_textarea_set_placeholder_text(ui_CAN_Send_Len, "Length...");
    lv_obj_set_style_bg_opa(ui_CAN_Send_Data, 255, LV_PART_CURSOR | LV_STATE_DEFAULT);
    lv_textarea_set_placeholder_text(ui_CAN_Send_Data, "");
  }
  // Triggered when the data field loses focus
  if (event_code == LV_EVENT_DEFOCUSED) {
    _ui_flag_modify(ui_CAN_Keyboard0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD); // Hide keyboard

    lv_obj_set_style_bg_opa(ui_CAN_Send_Data, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);
    lv_textarea_set_placeholder_text(ui_CAN_Send_Data, "Data(Hex)...");
  }
  // Triggered when the input is ready
  if (event_code == LV_EVENT_READY) {
    CANSend1(e); // Send the CAN message data

    lv_obj_set_style_bg_opa(ui_CAN_Send_ID, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);
    lv_textarea_set_placeholder_text(ui_CAN_Send_ID, "ID...");
    lv_obj_set_style_bg_opa(ui_CAN_Send_Len, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);
    lv_textarea_set_placeholder_text(ui_CAN_Send_Len, "Length...");
    lv_obj_set_style_bg_opa(ui_CAN_Send_Data, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);
    lv_textarea_set_placeholder_text(ui_CAN_Send_Data, "Data(Hex)...");
  }
}

// Event handler for toggling visibility of baud rate selection interface
void ui_event_CAN_Button2(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  // Triggered when the button is pressed
  if (event_code == LV_EVENT_PRESSED) {
    _ui_flag_modify(ui_CAN_Roller, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_TOGGLE); // Toggle visibility of baud rate roller
    _ui_flag_modify(ui_CAN_Button3, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_TOGGLE); // Toggle visibility of save button
  }
}

// Event handler for saving the selected CAN baud rate
void ui_event_CAN_Button3(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  // Triggered when the button is clicked
  if (event_code == LV_EVENT_CLICKED) {
    CANSavebaud(e);                                                           // Save the selected CAN baud rate
    _ui_flag_modify(ui_CAN_Roller, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);  // Hide baud rate roller
    _ui_flag_modify(ui_CAN_Button3, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD); // Hide save button
  }
}

// Event handler to clear the CAN message data
void ui_event_CAN_Button4(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  // Triggered when the button is clicked
  if (event_code == LV_EVENT_CLICKED) {
    CANClear(e); // Clear the CAN data
  }
}
