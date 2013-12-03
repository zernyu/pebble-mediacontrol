#include <pebble.h>

static Window *window;
static TextLayer *action_text_layer; // To display the user's actions
static TextLayer *connection_text_layer; // To display the current IP address
static ActionBarLayer *action_bar_layer; // To display the button icons
static GBitmap *action_pause_icon;
static GBitmap *action_loud_icon;
static GBitmap *action_quiet_icon;

enum {
  REQUEST_KEY,
  IP_ADDRESS_KEY
};

static void fail_handler(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "failed request: %d", reason);
}

/*
  The only message the watch receives from the phone is the IP address that
  requests will be sent to
*/
static void receive_handler(DictionaryIterator *iterator, void *context) {
  static char connection_text[64]; // Displayed in connection_text_layer

  Tuple *ip_address_tuple = dict_find(iterator, IP_ADDRESS_KEY);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "ip address: %s", ip_address_tuple->value->cstring);

  if (ip_address_tuple && strcmp(ip_address_tuple->value->cstring, "") != 0) {
    snprintf(connection_text, sizeof(connection_text), "Connected to: %s", ip_address_tuple->value->cstring);
    text_layer_set_text(connection_text_layer, connection_text);
  } else {
    snprintf(connection_text, sizeof(connection_text), "Please configure Media Control through the Pebble mobile app");
    text_layer_set_text(connection_text_layer, connection_text);
  }
}

static void send_request(char *request) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "sending request: %s", request);
  Tuplet request_tuple = TupletCString(REQUEST_KEY, request);

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  if (iter == NULL) {
    return;
  }

  dict_write_tuplet(iter, &request_tuple);
  dict_write_end(iter);

  app_message_outbox_send();
}

static void revert_text(void *data) {
  text_layer_set_text(action_text_layer, "");
}

static void pause_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(action_text_layer, "play/pause");
  send_request("pause");
  app_timer_register(1000, revert_text, NULL);
}

static void next_song_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(action_text_layer, "next");
  send_request("next");
  app_timer_register(1000, revert_text, NULL);
}

static void volume_up_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(action_text_layer, "volume up");
  send_request("volup");
  app_timer_register(1000, revert_text, NULL);
}

static void volume_down_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(action_text_layer, "volume down");
  send_request("voldown");
  app_timer_register(1000, revert_text, NULL);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, pause_handler);
  window_single_click_subscribe(BUTTON_ID_UP, volume_up_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, volume_down_handler);
  window_multi_click_subscribe(BUTTON_ID_SELECT, 2, 0, 100, false, next_song_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Text layers have to be at least 20 pixels narrower because of the action bar
  connection_text_layer = text_layer_create((GRect) { .origin = { 0, 10}, .size = { bounds.size.w - 30, 70 } });
  text_layer_set_text(connection_text_layer, "Loading Connection...");
  text_layer_set_text_alignment(connection_text_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(connection_text_layer, GTextOverflowModeFill);
  layer_add_child(window_layer, text_layer_get_layer(connection_text_layer));

  action_text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w - 30, 20 } });
  text_layer_set_text(action_text_layer, "");
  text_layer_set_text_alignment(action_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(action_text_layer));

  action_bar_layer = action_bar_layer_create();
  action_pause_icon = gbitmap_create_with_resource(RESOURCE_ID_ACTION_PAUSE_ICON);
  action_bar_layer_set_icon(action_bar_layer, BUTTON_ID_SELECT, action_pause_icon);
  action_loud_icon = gbitmap_create_with_resource(RESOURCE_ID_ACTION_LOUD_ICON);
  action_bar_layer_set_icon(action_bar_layer, BUTTON_ID_UP, action_loud_icon);
  action_quiet_icon = gbitmap_create_with_resource(RESOURCE_ID_ACTION_QUIET_ICON);
  action_bar_layer_set_icon(action_bar_layer, BUTTON_ID_DOWN, action_quiet_icon);
  action_bar_layer_set_click_config_provider(action_bar_layer, click_config_provider);
  action_bar_layer_add_to_window(action_bar_layer, window);
}

static void window_unload(Window *window) {
  text_layer_destroy(action_text_layer);
  text_layer_destroy(connection_text_layer);

  gbitmap_destroy(action_pause_icon);
  gbitmap_destroy(action_loud_icon);
  gbitmap_destroy(action_quiet_icon);
  action_bar_layer_destroy(action_bar_layer);
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload,
      });
  const bool animated = true;
  window_stack_push(window, animated);

  app_message_open(64, dict_calc_buffer_size(1, sizeof(char) * 8));
  app_message_register_outbox_failed(fail_handler);
  app_message_register_inbox_received(receive_handler);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
