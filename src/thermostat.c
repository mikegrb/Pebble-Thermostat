#include <pebble.h>

static Window *window;
static TextLayer *mode_layer;
static TextLayer *setpoint_layer;
static TextLayer *current_temp_layer;
static ActionBarLayer *action_bar;
static char str_new_setpoint[4];

enum {
  DATA_KEY_FETCH    = 0x0,
  DATA_KEY_TEMP     = 0x1,
  DATA_KEY_MODE     = 0x2,
  DATA_KEY_SET      = 0x3,
  RESOUCE_ICON_UP   = 0x1,
  RESOUCE_ICON_DOWN = 0x2,
}; 

void out_sent_handler(DictionaryIterator *sent, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message sent successfully!");
}

void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Failed to Send!");
}

void in_received_handler(DictionaryIterator *received, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Received!");
  Tuple *temp_tuple = dict_find(received, DATA_KEY_TEMP);
  Tuple *setpoint_tuple = dict_find(received, DATA_KEY_SET);
  Tuple *mode_tuple = dict_find(received, DATA_KEY_MODE);

  if (temp_tuple) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Temp: %s, Setpoint: %s, Mode: %s",
        temp_tuple->value->cstring, setpoint_tuple->value->cstring,
        mode_tuple->value->cstring);

    text_layer_set_text(current_temp_layer, temp_tuple->value->cstring);
    text_layer_set_text(setpoint_layer, setpoint_tuple->value->cstring);
    text_layer_set_text(mode_layer, mode_tuple->value->cstring);
  }
}

void in_dropped_handler(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Dropped!");
}

static void fetch_msg(void) {
  text_layer_set_text(current_temp_layer, "---");
  text_layer_set_text(mode_layer, "Fetching...");
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  Tuplet value = TupletInteger(0, 1);
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
}

static int myatoi(const char *s) {
  const char *p = s, *q;
  int n = 0;
  int sign = 1, k = 1;
  if (p != NULL) {
    if (*p != '\0') {
      if ((*p == '+') || (*p == '-')) {
        if (*p++ == '-') sign = -1;
      }
      for (q = p; (*p != '\0'); p++);
      for (--p; p >= q; --p, k *= 10) n += (*p - '0') * k;
    }
  }
  return n * sign;
}

static void change_setpoint (int change) {
  int current_value = myatoi(text_layer_get_text(setpoint_layer));
  int new_setpoint = current_value + change;
  snprintf(str_new_setpoint, sizeof(str_new_setpoint), "%i", new_setpoint);
  text_layer_set_text(setpoint_layer, str_new_setpoint);
  // TODO: also actually, you know, set it
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  fetch_msg();
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  change_setpoint( 1 );
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  change_setpoint( -1 );
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  current_temp_layer = text_layer_create(
      (GRect) { .origin = { 0, 20 }, .size = { bounds.size.w - 20, 50 } });
  text_layer_set_text_alignment(current_temp_layer, GTextAlignmentCenter);
  text_layer_set_font(current_temp_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(current_temp_layer));

  setpoint_layer = text_layer_create(
      (GRect) { .origin = { 0, 70 }, .size = { bounds.size.w - 20, 50 } });
  text_layer_set_text_alignment(setpoint_layer, GTextAlignmentCenter);
  text_layer_set_font(setpoint_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  layer_add_child(window_layer, text_layer_get_layer(setpoint_layer));

  mode_layer = text_layer_create(
      (GRect) { .origin = { 0, 100 }, .size = { bounds.size.w - 20, 50 } });
  text_layer_set_text_alignment(mode_layer, GTextAlignmentCenter);
  text_layer_set_font(mode_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  layer_add_child(window_layer, text_layer_get_layer(mode_layer));

  action_bar = action_bar_layer_create();
  action_bar_layer_add_to_window(action_bar, window);
  action_bar_layer_set_click_config_provider(action_bar, click_config_provider);

  action_bar_layer_set_icon(action_bar, BUTTON_ID_UP, gbitmap_create_with_resource( RESOUCE_ICON_UP ));
  action_bar_layer_set_icon(action_bar, BUTTON_ID_DOWN, gbitmap_create_with_resource( RESOUCE_ICON_DOWN ));

  fetch_msg();
}

static void window_unload(Window *window) {
  text_layer_destroy(mode_layer);
  text_layer_destroy(setpoint_layer);
  text_layer_destroy(current_temp_layer);
}

static void app_message_init(void) {
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_sent(out_sent_handler);
  app_message_register_outbox_failed(out_failed_handler);
  app_message_open(64, 64);
}

static void init(void) {
  app_message_init();
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
