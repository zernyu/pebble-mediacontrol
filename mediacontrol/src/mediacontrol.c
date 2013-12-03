#include <pebble.h>

static Window *window;
static TextLayer *text_layer;

enum {
  REQUEST_KEY = 0x0
};

static void fail_handler(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "failed request: %d", reason);
}

static void send_handler(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "success!");
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
  APP_LOG(APP_LOG_LEVEL_DEBUG, "should've sent?");
}

static void revert_text(void *data) {
  text_layer_set_text(text_layer, "dat music");
}

static void pause_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "play/pause");
  send_request("pause");
  app_timer_register(1000, revert_text, NULL);
}

static void next_song_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "next");
  send_request("next");
  app_timer_register(1000, revert_text, NULL);
}

static void volume_up_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "volume up");
  send_request("volup");
  app_timer_register(1000, revert_text, NULL);
}

static void volume_down_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "volume down");
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

  text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "dat music");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload,
      });
  const bool animated = true;
  window_stack_push(window, animated);

  app_message_open(4, dict_calc_buffer_size(1, sizeof(char) * 8));
  app_message_register_outbox_sent(send_handler);
  app_message_register_outbox_failed(fail_handler);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
