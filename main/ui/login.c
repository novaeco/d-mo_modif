#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "can.h"
#include "pwm.h"
#include "rs485.h"
#include "ui.h"
#include "ui_helpers.h"
#include "wifi.h"

// SCREEN: ui_Login
void ui_Login_screen_init(void);
lv_obj_t* ui_Login;
lv_obj_t* ui_Login_Panel1;
lv_obj_t* ui_Login_Keyboard;
lv_obj_t* ui_Login_Panel2;
void ui_event_Login_Label0(lv_event_t* e);
lv_obj_t* ui_Login_Label0;
lv_obj_t* ui_Login_Label1;
lv_obj_t* ui_Login_Label2;
void ui_event_Login_InputUser(lv_event_t* e);
lv_obj_t* ui_Login_InputUser;
void ui_event_Login_InputPass(lv_event_t* e);
lv_obj_t* ui_Login_InputPass;
void ui_event_Login_InputPass_EYE(lv_event_t* e);
lv_obj_t* ui_Login_InputPass_EYE;
lv_obj_t* ui_Login_Panel3;
lv_obj_t* ui_Login_Label3;

// CUSTOM VARIABLES for Login Screen
bool WIFI_INPUT_PWD_FLAG = true; // Flag to check if password input is visible

char saved_username[MAX_LENGTH] = "";   // Store the saved username
char saved_password[MAX_LENGTH] = "";   // Store the saved password
char entered_username[MAX_LENGTH] = ""; // Store the entered username
char entered_password[MAX_LENGTH] = ""; // Store the entered password

void ui_event_Login_Label0(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  if (event_code == LV_EVENT_CLICKED) {
    // Change screen to ui_Create_Uesr without animation and initialize it
    _ui_screen_change(&ui_Create_Uesr, LV_SCR_LOAD_ANIM_NONE, 0, 0, &ui_Create_Uesr_screen_init);
  }
}

// Event handler for the Login InputUser field (handling focus, defocus, and ready events)
void ui_event_Login_InputUser(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  if (event_code == LV_EVENT_FOCUSED) {
    // When focused, set the keyboard target to InputUser and show the keyboard
    _ui_keyboard_set_target(ui_Login_Keyboard, ui_Login_InputUser);
    _ui_flag_modify(ui_Login_Keyboard, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);

    lv_textarea_set_placeholder_text(ui_Login_InputUser, "");
    lv_obj_set_style_bg_opa(ui_Login_InputUser, 255, LV_PART_CURSOR | LV_STATE_DEFAULT);
  }
  if (event_code == LV_EVENT_DEFOCUSED) {
    // When defocused, hide the keyboard and hide the additional UI elements
    _ui_flag_modify(ui_Login_Keyboard, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_flag_modify(ui_Login_Panel3, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);

    lv_textarea_set_placeholder_text(ui_Login_InputUser, "Enter username...");
    lv_obj_set_style_bg_opa(ui_Login_InputUser, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);

    lv_textarea_set_placeholder_text(ui_Login_InputPass, "Enter password...");
    lv_obj_set_style_bg_opa(ui_Login_InputPass, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);
  }
  if (event_code == LV_EVENT_READY) {
    // When the user is done entering data, handle the input and move focus to the next field
    // input_user_event_cb(e);
    _ui_keyboard_set_target(ui_Login_Keyboard, ui_Login_InputPass);
    _ui_state_modify(ui_Login_InputUser, LV_STATE_FOCUSED, _UI_MODIFY_STATE_REMOVE);
    _ui_state_modify(ui_Login_InputPass, LV_STATE_FOCUSED, _UI_MODIFY_STATE_ADD);

    lv_textarea_set_placeholder_text(ui_Login_InputUser, "Enter username...");
    lv_obj_set_style_bg_opa(ui_Login_InputUser, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);

    lv_textarea_set_placeholder_text(ui_Login_InputPass, "");
    lv_obj_set_style_bg_opa(ui_Login_InputPass, 255, LV_PART_CURSOR | LV_STATE_DEFAULT);
  }
}

// Event handler for the Login InputPass field (handling focus, defocus, and ready events)
void ui_event_Login_InputPass(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  if (event_code == LV_EVENT_FOCUSED) {
    // When focused, show the keyboard and set the target to InputPass
    _ui_flag_modify(ui_Login_Keyboard, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    _ui_keyboard_set_target(ui_Login_Keyboard, ui_Login_InputPass);

    lv_textarea_set_placeholder_text(ui_Login_InputPass, "");
    lv_obj_set_style_bg_opa(ui_Login_InputPass, 255, LV_PART_CURSOR | LV_STATE_DEFAULT);
  }
  if (event_code == LV_EVENT_DEFOCUSED) {
    // When defocused, hide the keyboard and additional UI elements
    _ui_flag_modify(ui_Login_Keyboard, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_flag_modify(ui_Login_Panel3, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);

    lv_textarea_set_placeholder_text(ui_Login_InputUser, "Enter username...");
    lv_obj_set_style_bg_opa(ui_Login_InputUser, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);

    lv_textarea_set_placeholder_text(ui_Login_InputPass, "Enter password...");
    lv_obj_set_style_bg_opa(ui_Login_InputPass, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);
  }
  if (event_code == LV_EVENT_READY) {
    // When ready (password entered), handle the input and hide the keyboard
    input_user_event_cb(e);
    input_password_event_cb(e);
    _ui_state_modify(ui_Login_InputPass, LV_STATE_FOCUSED, _UI_MODIFY_STATE_REMOVE);
    _ui_flag_modify(ui_Login_Keyboard, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);

    lv_textarea_set_placeholder_text(ui_Login_InputPass, "Enter password...");
    lv_obj_set_style_bg_opa(ui_Login_InputPass, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);
  }
}

// Event handler for the Login InputPass_EYE button (to toggle password visibility)
void ui_event_Login_InputPass_EYE(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  if (event_code == LV_EVENT_CLICKED) {
    // Toggle the password visibility flag and update the password mode
    WIFI_INPUT_PWD_FLAG = !WIFI_INPUT_PWD_FLAG;
    lv_textarea_set_password_mode(ui_Login_InputPass, WIFI_INPUT_PWD_FLAG);
    _ui_state_modify(ui_Login_InputPass, LV_STATE_FOCUSED, _UI_MODIFY_STATE_ADD);
    _ui_keyboard_set_target(ui_Login_Keyboard, ui_Login_InputPass);
    _ui_flag_modify(ui_Login_Keyboard, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
  }
}
