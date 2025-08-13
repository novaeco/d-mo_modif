#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "can.h"
#include "pwm.h"
#include "rs485.h"
#include "ui.h"
#include "ui_helpers.h"
#include "wifi.h"

// SCREEN: ui_Main_WIFI
void ui_Main_WIFI_screen_init(void);
lv_obj_t* ui_Main_WIFI;
lv_obj_t* ui_WIFI;

lv_obj_t* ui_WIFI_STA;
lv_obj_t* ui_WIFI_SCAN_STA;
void ui_event_WIFI_Button0(lv_event_t* e);
lv_obj_t* ui_WIFI_Button0;
lv_obj_t* ui_WIFI_Label0;
void ui_event_WIFI_OPEN(lv_event_t* e);
lv_obj_t* ui_WIFI_OPEN;
lv_obj_t* ui_WIFI_STA_Label;
lv_obj_t* ui_WIFI_List_Win;
lv_obj_t* ui_WIFI_Spinner;
lv_obj_t* ui_WIFI_Details_Win;
lv_obj_t* ui_WIFI_Name;
lv_obj_t* ui_WIFI_Aurhmode;
lv_obj_t* ui_WIFI_Pairwise;
lv_obj_t* ui_WIFI_Group;
lv_obj_t* ui_WIFI_Channel;
lv_obj_t* ui_WIFI_IP;
void ui_event_WIFI_Connection_BUTTON(lv_event_t* e);
lv_obj_t* ui_WIFI_Connection_BUTTON;
lv_obj_t* ui_Connection;
void ui_event_WIFI_INPUT_PWD(lv_event_t* e);
lv_obj_t* ui_WIFI_INPUT_PWD;
void ui_event_WIFI_EYE(lv_event_t* e);
lv_obj_t* ui_WIFI_EYE;
lv_obj_t* ui_WIFI_INPUT_KEYBOARD;
lv_obj_t* ui_WIFI_Wait_CONNECTION;
lv_obj_t* ui_WIFI_PWD_Error;

lv_obj_t* ui_WIFI_AP;
lv_obj_t* ui_WIFI_OPEN_AP_;
void ui_event_WIFI_Button2(lv_event_t* e);
lv_obj_t* ui_WIFI_Button2;
lv_obj_t* ui_WIFI_Label2;
void ui_event_WIFI_AP_OPEN(lv_event_t* e);
lv_obj_t* ui_WIFI_AP_OPEN;
lv_obj_t* ui_WIFI_AP_INPUT_ERROR;
lv_obj_t* ui_WIFI_AP_Information;
void ui_event_WIFI_AP_NAME(lv_event_t* e);
lv_obj_t* ui_WIFI_AP_NAME;
void ui_event_WIFI_AP_Password(lv_event_t* e);
lv_obj_t* ui_WIFI_AP_Password;
void ui_event_WIFI_AP_EYE(lv_event_t* e);
lv_obj_t* ui_WIFI_AP_EYE;
void ui_event_WIFI_AP_Channel(lv_event_t* e);
lv_obj_t* ui_WIFI_AP_Channel;
lv_obj_t* ui_WIFI_AP_NAME_Label;
lv_obj_t* ui_WIFI_AP_PWD_Label;
lv_obj_t* ui_WIFI_AP_Channel_Label;
lv_obj_t* ui_WIFI_AP_MAC_ADDR;
lv_obj_t* ui_WIFI_AP_CON_NUM;
lv_obj_t* ui_WIFI_AP_Keyboard;
// CUSTOM VARIABLES
bool WIFI_DIS_PWD = true;
bool WIFI_AP_PWD = true;
int8_t WIFI_CONNECTION = -1;
int wifi_index = 0;

lv_obj_t* ui_WIFI_SCAN_List;
lv_obj_t* WIFI_List_Button;

lv_obj_t* ui_WIFI_AP_MAC_List;

bool WIFI_CONNECTION_DONE = false;
void ui_WIFI_list_event_cb(lv_event_t* e);

void ui_event_Wifi(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  // Triggered when the Wifi button is clicked
  if (event_code == LV_EVENT_CLICKED) {
    // Change the screen to the Wifi settings screen with a fade animation
    _ui_screen_change(&ui_Main_WIFI, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, &ui_Main_WIFI_screen_init);
  }
}

// Event handler for the Wifi back button (returns to main screen)
void ui_event_WIFI_Button0(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  // Triggered when the back button is clicked
  if (event_code == LV_EVENT_CLICKED) {
    // Change the screen back to the main screen with a fade animation
    _ui_screen_change(&ui_Main, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, &ui_Main_screen_init);
  }
}

// Event handler for toggling the Wifi switch (open/close Wifi functionality)
void ui_event_WIFI_OPEN(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);
  lv_obj_t* target = lv_event_get_target(e);

  // When the switch is turned ON (checked state)
  if (event_code == LV_EVENT_VALUE_CHANGED && lv_obj_has_state(target, LV_STATE_CHECKED)) {
    // Remove the hidden flag from the Wifi scan list (show the list)
    _ui_flag_modify(ui_WIFI_SCAN_List, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    WIFIOPEN(e); // Open Wifi functionality
  }

  // When the switch is turned OFF (unchecked state)
  if (event_code == LV_EVENT_VALUE_CHANGED && !lv_obj_has_state(target, LV_STATE_CHECKED)) {
    // Add the hidden flag to the Wifi scan list (hide the list)
    _ui_flag_modify(ui_WIFI_SCAN_List, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    WIFICLOSE(e); // Close Wifi functionality
  }
}

// Event handler for selecting a Wifi network from the scan list
void ui_WIFI_list_event_cb(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  // When a network in the list is pressed
  if (event_code == LV_EVENT_PRESSED) {

    // Show the Wifi details window and hide the password error window
    _ui_flag_modify(ui_WIFI_Details_Win, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    _ui_flag_modify(ui_WIFI_PWD_Error, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);

    // Get the button that was pressed (the selected Wifi network)
    WIFI_List_Button = lv_event_get_target(e);
    wifi_index = (int)lv_event_get_user_data(e); // Get the index of the selected Wifi network
    printf("index:%d\r\n", wifi_index);

    // If the selected network is the currently connected one, show the IP address
    if (WIFI_CONNECTION == wifi_index) {
      _ui_flag_modify(ui_WIFI_IP, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    } else {
      // Otherwise, hide the IP address display
      _ui_flag_modify(ui_WIFI_IP, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    }

    // Clear the password input field
    lv_textarea_set_text(ui_WIFI_INPUT_PWD, "");

    // Display Wifi details window
    _ui_flag_modify(ui_WIFI_Details_Win, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);

    static char result[100];
    // Display SSID
    sprintf(result, "SSID %s", ap_info[wifi_index].ssid);
    lv_label_set_text(ui_WIFI_Name, result);

    // Print authentication mode (e.g., WPA2, WPA3)
    print_auth_mode(ap_info[wifi_index].authmode);

    // If the network is not using WEP, display cipher types
    if (ap_info[wifi_index].authmode != WIFI_AUTH_WEP) {
      print_cipher_type(ap_info[wifi_index].pairwise_cipher, ap_info[wifi_index].group_cipher);
    }

    // Display the Wifi channel
    sprintf(result, "Channel \t\t%d", ap_info[wifi_index].primary);
    lv_label_set_text(ui_WIFI_Channel, result);
  }
}

/******************WIFI CONNECTION HANDLERS*********************** */

// Event handler for the Wifi connection button (shows the password input field)
void ui_event_WIFI_Connection_BUTTON(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  // Triggered when the connection button is clicked
  if (event_code == LV_EVENT_CLICKED) {
    // Show the password input field
    _ui_flag_modify(ui_WIFI_INPUT_PWD, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
  }
}

// Event handler for the Wifi password input field (handles keyboard interactions)
void ui_event_WIFI_INPUT_PWD(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  // Triggered when the password input field loses focus
  if (event_code == LV_EVENT_DEFOCUSED) {
    // Hide the keyboard and the password input field when it loses focus
    _ui_flag_modify(ui_WIFI_INPUT_KEYBOARD, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_flag_modify(ui_WIFI_INPUT_PWD, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
  }

  // Triggered when the password input field gains focus
  if (event_code == LV_EVENT_FOCUSED) {
    // Set the keyboard to target the password input field
    _ui_keyboard_set_target(ui_WIFI_INPUT_KEYBOARD, ui_WIFI_INPUT_PWD);
    // Show the keyboard and hide the password input field
    _ui_flag_modify(ui_WIFI_INPUT_KEYBOARD, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
  }

  // Triggered when the user has finished entering the password
  if (event_code == LV_EVENT_READY) {
    // Attempt to connect to Wifi with the provided password
    WIFIConnection(e);
    // Hide the keyboard and password field after connection attempt
    _ui_flag_modify(ui_WIFI_INPUT_KEYBOARD, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_flag_modify(ui_WIFI_INPUT_PWD, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    // Show the waiting for connection window
    _ui_flag_modify(ui_WIFI_Wait_CONNECTION, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
  }
}

// Event handler for the eye icon (toggles password visibility)
void ui_event_WIFI_EYE(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  // Triggered when the eye icon is clicked
  if (event_code == LV_EVENT_CLICKED) {
    // Toggle password visibility
    WIFI_DIS_PWD = !WIFI_DIS_PWD;
    // Update the password input field based on the toggle state
    lv_textarea_set_password_mode(ui_WIFI_INPUT_PWD, WIFI_DIS_PWD);
    // Show the keyboard and password field
    _ui_flag_modify(ui_WIFI_INPUT_KEYBOARD, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    _ui_flag_modify(ui_WIFI_INPUT_PWD, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
  }
}

// Event handler for the back button in the Wifi settings (returns to the main screen)
void ui_event_WIFI_Button2(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  // Triggered when the back button is clicked
  if (event_code == LV_EVENT_CLICKED) {
    // Change the screen back to the main screen with a fade animation
    _ui_screen_change(&ui_Main, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, &ui_Main_screen_init);
  }
}

/******************WIFI AP HANDLERS*********************** */

// Event handler for the AP name input field
void ui_event_WIFI_AP_NAME(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  // Triggered when the AP name input field gains focus
  if (event_code == LV_EVENT_FOCUSED) {
    // Set the keyboard to target the AP name field
    _ui_keyboard_set_target(ui_WIFI_AP_Keyboard, ui_WIFI_AP_NAME);
    // Show the keyboard and hide the input error message
    _ui_flag_modify(ui_WIFI_AP_Keyboard, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    _ui_flag_modify(ui_WIFI_AP_INPUT_ERROR, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
  }

  // Triggered when the AP name input field loses focus
  if (event_code == LV_EVENT_DEFOCUSED) {
    // Hide the keyboard when the field loses focus
    _ui_flag_modify(ui_WIFI_AP_Keyboard, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
  }

  // Triggered when the user finishes entering the AP name
  if (event_code == LV_EVENT_READY) {
    // Change focus to the password field after entering the AP name
    _ui_state_modify(ui_WIFI_AP_NAME, LV_STATE_FOCUSED, _UI_MODIFY_STATE_REMOVE);
    _ui_state_modify(ui_WIFI_AP_Password, LV_STATE_FOCUSED, _UI_MODIFY_STATE_ADD);
    // Set the keyboard to target the password input field
    _ui_keyboard_set_target(ui_WIFI_AP_Keyboard, ui_WIFI_AP_Password);
  }
}

// Event handler for the AP password input field
void ui_event_WIFI_AP_Password(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  // Triggered when the password input field gains focus
  if (event_code == LV_EVENT_FOCUSED) {
    // Set the keyboard to target the password field
    _ui_keyboard_set_target(ui_WIFI_AP_Keyboard, ui_WIFI_AP_Password);
    // Show the keyboard and hide the input error message
    _ui_flag_modify(ui_WIFI_AP_Keyboard, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    _ui_flag_modify(ui_WIFI_AP_INPUT_ERROR, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
  }

  // Triggered when the password input field loses focus
  if (event_code == LV_EVENT_DEFOCUSED) {
    // Hide the keyboard when the field loses focus
    _ui_flag_modify(ui_WIFI_AP_Keyboard, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
  }

  // Triggered when the user finishes entering the password
  if (event_code == LV_EVENT_READY) {
    // Change focus to the AP channel field after entering the password
    _ui_state_modify(ui_WIFI_AP_Channel, LV_STATE_FOCUSED, _UI_MODIFY_STATE_ADD);
    _ui_state_modify(ui_WIFI_AP_Password, LV_STATE_FOCUSED, _UI_MODIFY_STATE_REMOVE);
    // Set the keyboard to target the AP channel input field
    _ui_keyboard_set_target(ui_WIFI_AP_Keyboard, ui_WIFI_AP_Channel);
  }
}

// Event handler for the eye icon (toggles the visibility of the AP password)
void ui_event_WIFI_AP_EYE(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  // Triggered when the eye icon is clicked
  if (event_code == LV_EVENT_CLICKED) {
    // Toggle password visibility
    WIFI_AP_PWD = !WIFI_AP_PWD;
    // Update the password input field based on the visibility state
    lv_textarea_set_password_mode(ui_WIFI_AP_Password, WIFI_AP_PWD);
    // Show the keyboard and password input field
    _ui_flag_modify(ui_WIFI_AP_Keyboard, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
  }
}

// Event handler for the AP channel input field
void ui_event_WIFI_AP_Channel(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  // Triggered when the AP channel input field gains focus
  if (event_code == LV_EVENT_FOCUSED) {
    // Set the keyboard to target the AP channel field
    _ui_keyboard_set_target(ui_WIFI_AP_Keyboard, ui_WIFI_AP_Channel);
    // Show the keyboard and hide the input error message
    _ui_flag_modify(ui_WIFI_AP_Keyboard, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    _ui_flag_modify(ui_WIFI_AP_INPUT_ERROR, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
  }

  // Triggered when the AP channel input field loses focus
  if (event_code == LV_EVENT_DEFOCUSED) {
    // Hide the keyboard when the field loses focus
    _ui_flag_modify(ui_WIFI_AP_Keyboard, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
  }

  // Triggered when the user finishes entering the AP channel
  if (event_code == LV_EVENT_READY) {
    // Hide the keyboard after the user is done entering the channel
    _ui_flag_modify(ui_WIFI_AP_Keyboard, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
  }
}

// Event handler for toggling the Wifi AP state (open/close)
void ui_event_WIFI_AP_OPEN(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);
  lv_obj_t* target = lv_event_get_target(e);

  // Triggered when the toggle state of the AP open button changes
  if (event_code == LV_EVENT_VALUE_CHANGED && lv_obj_has_state(target, LV_STATE_CHECKED)) {
    // If checked, open the Wifi AP
    WIFIAPOPEN(e);
  }

  if (event_code == LV_EVENT_VALUE_CHANGED && !lv_obj_has_state(target, LV_STATE_CHECKED)) {
    // If unchecked, close the Wifi AP
    WIFIAPCLOSE(e);
  }
}
