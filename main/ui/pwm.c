#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "can.h"
#include "pwm.h"
#include "rs485.h"
#include "ui.h"
#include "ui_helpers.h"
#include "wifi.h"

// SCREEN: ui_Main_PWM
void ui_Main_PWM_screen_init(void);
lv_obj_t* ui_Main_PWM;
lv_obj_t* ui_PWM_Panel1;
void ui_event_PWM_Arc0(lv_event_t* e);
lv_obj_t* ui_PWM_Arc0;
lv_obj_t* ui_PWM_Label0;
lv_obj_t* ui_PWM_Label9;
lv_obj_t* ui_BAT_Label0;
lv_obj_t* ui_PWM_Panel2;
void ui_event_PWM_Slider0(lv_event_t* e);
lv_obj_t* ui_PWM_Slider0;
lv_obj_t* ui_PWM_Label1;
lv_obj_t* ui_PWM_Panel3;
void ui_event_PWM_Switch1(lv_event_t* e);
lv_obj_t* ui_PWM_Switch1;
void ui_event_PWM_Switch2(lv_event_t* e);
lv_obj_t* ui_PWM_Switch2;
void ui_event_PWM_Switch3(lv_event_t* e);
lv_obj_t* ui_PWM_Switch3;
lv_obj_t* ui_PWM_Label2;
lv_obj_t* ui_PWM_Label3;
lv_obj_t* ui_PWM_Label4;
lv_obj_t* ui_PWM_Panel4;
lv_obj_t* ui_PWM_Label5;
lv_obj_t* ui_PWM_Label6;
lv_obj_t* ui_PWM_Label7;
void ui_event_PWM_Button0(lv_event_t* e);
lv_obj_t* ui_PWM_Button0;
lv_obj_t* ui_PWM_Label8;
// CUSTOM VARIABLES

void ui_event_PWM(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  // Triggered when the event is a click
  if (event_code == LV_EVENT_CLICKED) {
    // Change screen to the PWM control screen
    _ui_screen_change(&ui_Main_PWM, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, &ui_Main_PWM_screen_init);
  }
}

// Event handler for handling interactions with the PWM Arc control (long press and click)
void ui_event_PWM_Arc0(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);
  lv_obj_t* target = lv_event_get_target(e);

  // Triggered during a long press repeat on the Arc control
  if (event_code == LV_EVENT_LONG_PRESSED_REPEAT) {
    // Clear the text value of the label next to the arc control
    _ui_arc_set_text_value(ui_PWM_Label0, target, "", "");

    // Get the current text of the label, then update the other label
    const char* text = lv_label_get_text(ui_PWM_Label0);
    lv_label_set_text(ui_PWM_Label1, text);

    // Get the value of the arc and set the value on the corresponding slider
    int16_t value = lv_arc_get_value(ui_PWM_Arc0);
    lv_slider_set_value(ui_PWM_Slider0, value, LV_ANIM_ON);

    // Set the PWM value by adjusting it (100 - arc value)
    IO_EXTENSION_Pwm_Output(100 - value);
    DEV_SET_PWM(100 - value);
  }

  // Triggered when the Arc control is clicked
  if (event_code == LV_EVENT_CLICKED) {
    // Clear the text value of the label next to the arc control
    _ui_arc_set_text_value(ui_PWM_Label0, target, "", "");

    // Get the current text of the label, then update the other label
    const char* text = lv_label_get_text(ui_PWM_Label0);
    lv_label_set_text(ui_PWM_Label1, text);

    // Get the value of the arc and set the value on the corresponding slider
    int16_t value = lv_arc_get_value(ui_PWM_Arc0);
    lv_slider_set_value(ui_PWM_Slider0, value, LV_ANIM_ON);

    // Set the PWM value by adjusting it (100 - arc value)
    IO_EXTENSION_Pwm_Output(100 - value);
    DEV_SET_PWM(100 - value);
  }
}

// Event handler for the PWM slider (long press and click)
void ui_event_PWM_Slider0(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);
  lv_obj_t* target = lv_event_get_target(e);

  // Triggered during a long press repeat on the slider control
  if (event_code == LV_EVENT_LONG_PRESSED_REPEAT) {
    // Clear the text value of the label next to the slider
    _ui_slider_set_text_value(ui_PWM_Label1, target, "", "");

    // Get the current text of the label, then update the other label
    const char* text = lv_label_get_text(ui_PWM_Label1);
    lv_label_set_text(ui_PWM_Label0, text);

    // Get the value of the slider and set the value on the corresponding arc control
    int16_t value = lv_slider_get_value(ui_PWM_Slider0);
    lv_arc_set_value(ui_PWM_Arc0, value);

    // Set the PWM value by adjusting it (100 - slider value)
    IO_EXTENSION_Pwm_Output(100 - value);
    DEV_SET_PWM(100 - value);
  }

  // Triggered when the slider is clicked
  if (event_code == LV_EVENT_CLICKED) {
    // Clear the text value of the label next to the slider
    _ui_slider_set_text_value(ui_PWM_Label1, target, "", "");

    // Get the current text of the label, then update the other label
    const char* text = lv_label_get_text(ui_PWM_Label1);
    lv_label_set_text(ui_PWM_Label0, text);

    // Get the value of the slider and set the value on the corresponding arc control
    int16_t value = lv_slider_get_value(ui_PWM_Slider0);
    lv_arc_set_value(ui_PWM_Arc0, value);

    // Set the PWM value by adjusting it (100 - slider value)
    IO_EXTENSION_Pwm_Output(100 - value);
    DEV_SET_PWM(100 - value);
  }
}

// Event handler for the first PWM switch (Switch1)
void ui_event_PWM_Switch1(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);
  lv_obj_t* target = lv_event_get_target(e);

  // Triggered when the switch state changes to "checked"
  if (event_code == LV_EVENT_VALUE_CHANGED && lv_obj_has_state(target, LV_STATE_CHECKED)) {
    // Disable other switches to prevent conflicts
    _ui_state_modify(ui_PWM_Switch2, LV_STATE_DISABLED, _UI_MODIFY_STATE_ADD);
    _ui_state_modify(ui_PWM_Switch3, LV_STATE_DISABLED, _UI_MODIFY_STATE_ADD);

    // Set values for the Arc, Slider, and Labels to indicate PWM level
    lv_arc_set_value(ui_PWM_Arc0, 33);
    lv_slider_set_value(ui_PWM_Slider0, 33, LV_ANIM_ON);
    lv_label_set_text(ui_PWM_Label0, "33");
    lv_label_set_text(ui_PWM_Label1, "33");

    // Set the PWM output to 33
    IO_EXTENSION_Pwm_Output(100 - 33);
    DEV_SET_PWM(100 - 33);
  }

  // Triggered when the switch state changes to "unchecked"
  if (event_code == LV_EVENT_VALUE_CHANGED && !lv_obj_has_state(target, LV_STATE_CHECKED)) {
    // Enable other switches to allow interaction
    _ui_state_modify(ui_PWM_Switch3, LV_STATE_DISABLED, _UI_MODIFY_STATE_REMOVE);
    _ui_state_modify(ui_PWM_Switch2, LV_STATE_DISABLED, _UI_MODIFY_STATE_REMOVE);

    // Reset all values to 50
    lv_arc_set_value(ui_PWM_Arc0, 50);
    lv_slider_set_value(ui_PWM_Slider0, 50, LV_ANIM_ON);
    lv_label_set_text(ui_PWM_Label0, "50");
    lv_label_set_text(ui_PWM_Label1, "50");

    // Set PWM output to 100 (off)
    IO_EXTENSION_Pwm_Output(50);
    DEV_SET_PWM(50);
  }
}

// Event handler for the second PWM switch (Switch2)
void ui_event_PWM_Switch2(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);
  lv_obj_t* target = lv_event_get_target(e);

  // Triggered when the switch state changes to "checked"
  if (event_code == LV_EVENT_VALUE_CHANGED && lv_obj_has_state(target, LV_STATE_CHECKED)) {
    // Disable other switches to prevent conflicts
    _ui_state_modify(ui_PWM_Switch3, LV_STATE_DISABLED, _UI_MODIFY_STATE_ADD);
    _ui_state_modify(ui_PWM_Switch1, LV_STATE_DISABLED, _UI_MODIFY_STATE_ADD);

    // Set values for the Arc, Slider, and Labels to indicate PWM level
    lv_arc_set_value(ui_PWM_Arc0, 66);
    lv_slider_set_value(ui_PWM_Slider0, 66, LV_ANIM_ON);
    lv_label_set_text(ui_PWM_Label0, "66");
    lv_label_set_text(ui_PWM_Label1, "66");

    // Set the PWM output to 66
    IO_EXTENSION_Pwm_Output(100 - 66);
    DEV_SET_PWM(100 - 66);
  }

  // Triggered when the switch state changes to "unchecked"
  if (event_code == LV_EVENT_VALUE_CHANGED && !lv_obj_has_state(target, LV_STATE_CHECKED)) {
    // Enable other switches to allow interaction
    _ui_state_modify(ui_PWM_Switch3, LV_STATE_DISABLED, _UI_MODIFY_STATE_REMOVE);
    _ui_state_modify(ui_PWM_Switch1, LV_STATE_DISABLED, _UI_MODIFY_STATE_REMOVE);

    // Reset all values to 50
    lv_arc_set_value(ui_PWM_Arc0, 50);
    lv_slider_set_value(ui_PWM_Slider0, 50, LV_ANIM_ON);
    lv_label_set_text(ui_PWM_Label0, "50");
    lv_label_set_text(ui_PWM_Label1, "50");

    // Set PWM output to 100 (off)
    IO_EXTENSION_Pwm_Output(50);
    DEV_SET_PWM(50);
  }
}

// Event handler for the third PWM switch (Switch3)
void ui_event_PWM_Switch3(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);
  lv_obj_t* target = lv_event_get_target(e);

  // Triggered when the switch state changes to "checked"
  if (event_code == LV_EVENT_VALUE_CHANGED && lv_obj_has_state(target, LV_STATE_CHECKED)) {
    // Disable other switches to prevent conflicts
    _ui_state_modify(ui_PWM_Switch2, LV_STATE_DISABLED, _UI_MODIFY_STATE_ADD);
    _ui_state_modify(ui_PWM_Switch1, LV_STATE_DISABLED, _UI_MODIFY_STATE_ADD);

    // Set values for the Arc, Slider, and Labels to indicate PWM level
    lv_arc_set_value(ui_PWM_Arc0, 99);
    lv_slider_set_value(ui_PWM_Slider0, 99, LV_ANIM_ON);
    lv_label_set_text(ui_PWM_Label0, "99");
    lv_label_set_text(ui_PWM_Label1, "99");

    // Set the PWM output to 99
    IO_EXTENSION_Pwm_Output(100 - 99);
    DEV_SET_PWM(100 - 99);
  }

  // Triggered when the switch state changes to "unchecked"
  if (event_code == LV_EVENT_VALUE_CHANGED && !lv_obj_has_state(target, LV_STATE_CHECKED)) {
    // Enable other switches to allow interaction
    _ui_state_modify(ui_PWM_Switch2, LV_STATE_DISABLED, _UI_MODIFY_STATE_REMOVE);
    _ui_state_modify(ui_PWM_Switch1, LV_STATE_DISABLED, _UI_MODIFY_STATE_REMOVE);

    // Reset all values to 50
    lv_arc_set_value(ui_PWM_Arc0, 50);
    lv_slider_set_value(ui_PWM_Slider0, 50, LV_ANIM_ON);
    lv_label_set_text(ui_PWM_Label0, "50");
    lv_label_set_text(ui_PWM_Label1, "50");

    // Set PWM output to 100 (off)
    IO_EXTENSION_Pwm_Output(50);
    DEV_SET_PWM(50);
  }
}

// Event handler for the PWM Button (Button0)
void ui_event_PWM_Button0(lv_event_t* e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  // Triggered when the button is clicked
  if (event_code == LV_EVENT_CLICKED) {
    // Change the screen to the main screen with a fade animation
    _ui_screen_change(&ui_Main, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, &ui_Main_screen_init);
  }
}
