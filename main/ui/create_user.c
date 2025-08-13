#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "can.h"
#include "pwm.h"
#include "rs485.h"
#include "ui.h"
#include "ui_helpers.h"
#include "wifi.h"

// SCREEN: ui_Create_User
void ui_Create_Uesr_screen_init(void);
lv_obj_t* ui_Create_Uesr;
lv_obj_t* ui_Create_Uesr_Panel0;
lv_obj_t* ui_Create_Uesr_Label0;
lv_obj_t* ui_Create_Uesr_Label1;
void ui_event_Create_Uesr_Name(lv_event_t* e);
lv_obj_t* ui_Create_Uesr_Name;
void ui_event_Create_Uesr_Password(lv_event_t* e);
lv_obj_t* ui_Create_Uesr_Password;
void ui_event_Create_Uesr_Pwd_EYE(lv_event_t* e);
lv_obj_t* ui_Create_Uesr_Pwd_EYE;
lv_obj_t* ui_Create_Uesr_Keyboard0;
void ui_event_Create_Uesr_Button0(lv_event_t* e);
lv_obj_t* ui_Create_Uesr_Button0;
lv_obj_t* ui_Create_Uesr_Label2;
lv_obj_t* ui_Create_Uesr_Label3;

// CUSTOM VARIABLES for Create User Screen
bool WIFI_Create_PWD_FLAG = true; // Flag to check if password creation is visible

void ui_event_Create_Uesr_Name(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  if (event_code == LV_EVENT_FOCUSED) {
    // When focused, show the keyboard and set the target to Name input field
    _ui_keyboard_set_target(ui_Create_Uesr_Keyboard0, ui_Create_Uesr_Name);
    _ui_flag_modify(ui_Create_Uesr_Keyboard0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);

    lv_textarea_set_placeholder_text(ui_Create_Uesr_Name, "");
    lv_obj_set_style_bg_opa(ui_Create_Uesr_Name, 255, LV_PART_CURSOR | LV_STATE_DEFAULT);
  }
  if (event_code == LV_EVENT_DEFOCUSED) {
    // When defocused, hide the keyboard
    _ui_flag_modify(ui_Create_Uesr_Keyboard0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);

    lv_textarea_set_placeholder_text(ui_Create_Uesr_Name, "Enter username...");
    lv_obj_set_style_bg_opa(ui_Create_Uesr_Name, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);

    lv_textarea_set_placeholder_text(ui_Create_Uesr_Password, "Enter password...");
    lv_obj_set_style_bg_opa(ui_Create_Uesr_Password, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);
  }
  if (event_code == LV_EVENT_READY) {
    // When ready (name entered), handle the input and move to the next field

    _ui_keyboard_set_target(ui_Create_Uesr_Keyboard0, ui_Create_Uesr_Password);
    _ui_state_modify(ui_Create_Uesr_Name, LV_STATE_FOCUSED, _UI_MODIFY_STATE_REMOVE);
    _ui_state_modify(ui_Create_Uesr_Password, LV_STATE_FOCUSED, _UI_MODIFY_STATE_ADD);

    lv_textarea_set_placeholder_text(ui_Create_Uesr_Name, "Enter username...");
    lv_obj_set_style_bg_opa(ui_Create_Uesr_Name, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);

    lv_textarea_set_placeholder_text(ui_Create_Uesr_Password, "");
    lv_obj_set_style_bg_opa(ui_Create_Uesr_Password, 255, LV_PART_CURSOR | LV_STATE_DEFAULT);
  }
}

// Event handler for the Create User Password input field (handling focus, defocus, and ready events)
void ui_event_Create_Uesr_Password(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  if (event_code == LV_EVENT_FOCUSED) {
    // When focused, show the keyboard and set the target to Password input field
    _ui_flag_modify(ui_Create_Uesr_Keyboard0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    _ui_keyboard_set_target(ui_Create_Uesr_Keyboard0, ui_Create_Uesr_Password);

    lv_textarea_set_placeholder_text(ui_Create_Uesr_Password, "");
    lv_obj_set_style_bg_opa(ui_Create_Uesr_Password, 255, LV_PART_CURSOR | LV_STATE_DEFAULT);
  }
  if (event_code == LV_EVENT_DEFOCUSED) {
    // When defocused, hide the keyboard and additional UI elements
    _ui_flag_modify(ui_Create_Uesr_Keyboard0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_flag_modify(ui_Create_Uesr_Label2, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);

    lv_textarea_set_placeholder_text(ui_Create_Uesr_Name, "Enter username...");
    lv_obj_set_style_bg_opa(ui_Create_Uesr_Name, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);

    lv_textarea_set_placeholder_text(ui_Create_Uesr_Password, "Enter password...");
    lv_obj_set_style_bg_opa(ui_Create_Uesr_Password, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);
  }
  if (event_code == LV_EVENT_READY) {
    // When ready (password entered), handle the password input
    new_user_event_cb(e);
    mew_password_event_cb(e);

    lv_textarea_set_placeholder_text(ui_Create_Uesr_Name, "Enter username...");
    lv_obj_set_style_bg_opa(ui_Create_Uesr_Name, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);

    lv_textarea_set_placeholder_text(ui_Create_Uesr_Password, "Enter password...");
    lv_obj_set_style_bg_opa(ui_Create_Uesr_Password, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);
  }
}

// Event handler for toggling the visibility of the password in the Create User password field
void ui_event_Create_Uesr_Pwd_EYE(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  if (event_code == LV_EVENT_CLICKED) {
    // Toggle the password visibility flag and update the password mode
    WIFI_Create_PWD_FLAG = !WIFI_Create_PWD_FLAG;
    lv_textarea_set_password_mode(ui_Create_Uesr_Password, WIFI_Create_PWD_FLAG);
    _ui_state_modify(ui_Create_Uesr_Password, LV_STATE_FOCUSED, _UI_MODIFY_STATE_ADD);
    _ui_keyboard_set_target(ui_Create_Uesr_Keyboard0, ui_Create_Uesr_Password);
    _ui_flag_modify(ui_Create_Uesr_Keyboard0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
  }
}

// Event handler for the Create User button (used to create the user and clear fields)
void ui_event_Create_Uesr_Button0(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  if (event_code == LV_EVENT_CLICKED) {
    // Disable fields and clear the text input fields
    _ui_state_modify(ui_Create_Uesr_Name, LV_STATE_DISABLED, _UI_MODIFY_STATE_REMOVE);
    _ui_state_modify(ui_Create_Uesr_Password, LV_STATE_DISABLED, _UI_MODIFY_STATE_REMOVE);
    lv_textarea_set_text(ui_Create_Uesr_Name, "");     // Clear the username field
    lv_textarea_set_text(ui_Create_Uesr_Password, ""); // Clear the password field

    // Hide the current button and label, then navigate to the Login screen
    _ui_flag_modify(ui_Create_Uesr_Button0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_flag_modify(ui_Create_Uesr_Label2, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_screen_change(&ui_Login, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, &ui_Login_screen_init);
  }
}
void ui_event_Create_Uesr_Name(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  if (event_code == LV_EVENT_FOCUSED) {
    // When focused, show the keyboard and set the target to Name input field
    _ui_keyboard_set_target(ui_Create_Uesr_Keyboard0, ui_Create_Uesr_Name);
    _ui_flag_modify(ui_Create_Uesr_Keyboard0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);

    lv_textarea_set_placeholder_text(ui_Create_Uesr_Name, "");
    lv_obj_set_style_bg_opa(ui_Create_Uesr_Name, 255, LV_PART_CURSOR | LV_STATE_DEFAULT);
  }
  if (event_code == LV_EVENT_DEFOCUSED) {
    // When defocused, hide the keyboard
    _ui_flag_modify(ui_Create_Uesr_Keyboard0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);

    lv_textarea_set_placeholder_text(ui_Create_Uesr_Name, "Enter username...");
    lv_obj_set_style_bg_opa(ui_Create_Uesr_Name, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);

    lv_textarea_set_placeholder_text(ui_Create_Uesr_Password, "Enter password...");
    lv_obj_set_style_bg_opa(ui_Create_Uesr_Password, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);
  }
  if (event_code == LV_EVENT_READY) {
    // When ready (name entered), handle the input and move to the next field

    _ui_keyboard_set_target(ui_Create_Uesr_Keyboard0, ui_Create_Uesr_Password);
    _ui_state_modify(ui_Create_Uesr_Name, LV_STATE_FOCUSED, _UI_MODIFY_STATE_REMOVE);
    _ui_state_modify(ui_Create_Uesr_Password, LV_STATE_FOCUSED, _UI_MODIFY_STATE_ADD);

    lv_textarea_set_placeholder_text(ui_Create_Uesr_Name, "Enter username...");
    lv_obj_set_style_bg_opa(ui_Create_Uesr_Name, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);

    lv_textarea_set_placeholder_text(ui_Create_Uesr_Password, "");
    lv_obj_set_style_bg_opa(ui_Create_Uesr_Password, 255, LV_PART_CURSOR | LV_STATE_DEFAULT);
  }
}

// Event handler for the Create User Password input field (handling focus, defocus, and ready events)
void ui_event_Create_Uesr_Password(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  if (event_code == LV_EVENT_FOCUSED) {
    // When focused, show the keyboard and set the target to Password input field
    _ui_flag_modify(ui_Create_Uesr_Keyboard0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    _ui_keyboard_set_target(ui_Create_Uesr_Keyboard0, ui_Create_Uesr_Password);

    lv_textarea_set_placeholder_text(ui_Create_Uesr_Password, "");
    lv_obj_set_style_bg_opa(ui_Create_Uesr_Password, 255, LV_PART_CURSOR | LV_STATE_DEFAULT);
  }
  if (event_code == LV_EVENT_DEFOCUSED) {
    // When defocused, hide the keyboard and additional UI elements
    _ui_flag_modify(ui_Create_Uesr_Keyboard0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_flag_modify(ui_Create_Uesr_Label2, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);

    lv_textarea_set_placeholder_text(ui_Create_Uesr_Name, "Enter username...");
    lv_obj_set_style_bg_opa(ui_Create_Uesr_Name, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);

    lv_textarea_set_placeholder_text(ui_Create_Uesr_Password, "Enter password...");
    lv_obj_set_style_bg_opa(ui_Create_Uesr_Password, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);
  }
  if (event_code == LV_EVENT_READY) {
    // When ready (password entered), handle the password input
    new_user_event_cb(e);
    mew_password_event_cb(e);

    lv_textarea_set_placeholder_text(ui_Create_Uesr_Name, "Enter username...");
    lv_obj_set_style_bg_opa(ui_Create_Uesr_Name, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);

    lv_textarea_set_placeholder_text(ui_Create_Uesr_Password, "Enter password...");
    lv_obj_set_style_bg_opa(ui_Create_Uesr_Password, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);
  }
}

// Event handler for toggling the visibility of the password in the Create User password field
void ui_event_Create_Uesr_Pwd_EYE(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  if (event_code == LV_EVENT_CLICKED) {
    // Toggle the password visibility flag and update the password mode
    WIFI_Create_PWD_FLAG = !WIFI_Create_PWD_FLAG;
    lv_textarea_set_password_mode(ui_Create_Uesr_Password, WIFI_Create_PWD_FLAG);
    _ui_state_modify(ui_Create_Uesr_Password, LV_STATE_FOCUSED, _UI_MODIFY_STATE_ADD);
    _ui_keyboard_set_target(ui_Create_Uesr_Keyboard0, ui_Create_Uesr_Password);
    _ui_flag_modify(ui_Create_Uesr_Keyboard0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
  }
}

// Event handler for the Create User button (used to create the user and clear fields)
void ui_event_Create_Uesr_Button0(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  if (event_code == LV_EVENT_CLICKED) {
    // Disable fields and clear the text input fields
    _ui_state_modify(ui_Create_Uesr_Name, LV_STATE_DISABLED, _UI_MODIFY_STATE_REMOVE);
    _ui_state_modify(ui_Create_Uesr_Password, LV_STATE_DISABLED, _UI_MODIFY_STATE_REMOVE);
    lv_textarea_set_text(ui_Create_Uesr_Name, "");     // Clear the username field
    lv_textarea_set_text(ui_Create_Uesr_Password, ""); // Clear the password field

    // Hide the current button and label, then navigate to the Login screen
    _ui_flag_modify(ui_Create_Uesr_Button0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_flag_modify(ui_Create_Uesr_Label2, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_screen_change(&ui_Login, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, &ui_Login_screen_init);
  }
}
