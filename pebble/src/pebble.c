#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0xED, 0xB5, 0x02, 0xF4, 0xEC, 0xB4, 0x4C, 0x6E, 0x9E, 0xD6, 0x80, 0x71, 0x8C, 0x28, 0xC7, 0x98 }
PBL_APP_INFO(MY_UUID,
             "Sailor Watch", "Sasha Ognev",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_STANDARD_APP);

Window window;

AppSync sync;
uint8_t sync_buffer[64];

TextLayer timeLayer; // The clock
TextLayer dateLayer;
TextLayer speedLayer;
TextLayer weatherLayer;

enum {
  SPEED_KEY = 0x0,
  WIND_SPEED_KEY = 0x1,
};

char* buffers[] = {"Speed: 00.0 kts", "Wind -- (--) @ ---°"};
TextLayer* layers[] = {&speedLayer, &weatherLayer};


// Called once per minute
void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {

  (void)t;
  (void)ctx;

  static char timeText[] = "00:00"; // Needs to be static because it's used by the system later.
  static char dateText[] = "May 14";

  PblTm currentTime;

  get_time(&currentTime);
  string_format_time(timeText, sizeof(timeText), "%R", &currentTime);
  text_layer_set_text(&timeLayer, timeText);

  string_format_time(dateText, sizeof(dateText), "%b %d", &currentTime);
  text_layer_set_text(&dateLayer, dateText);
}

static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  (void) dict_error;
  (void) app_message_error;
  (void) context;
  text_layer_set_text(&weatherLayer, "Communication error");
}

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
  (void) old_tuple;

  switch (key) {
  case SPEED_KEY:
  case WIND_SPEED_KEY:
    strncpy(buffers[key], new_tuple->value->cstring, 32); // TODO: Buffer size without using sizeof(buffer[key]) that doesn't work in this case
    text_layer_set_text(layers[key], buffers[key]);
    break;
  default:
    return;
  }
}

void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Ahoy");
  window_set_fullscreen(&window, true);
  window_stack_push(&window, true /* Animated */);

  window_set_background_color(&window, GColorBlack);

  text_layer_init(&timeLayer, GRect(0, 20, 144 /* width */, 168-30 /* height */));
  text_layer_set_text_color(&timeLayer, GColorWhite);
  text_layer_set_background_color(&timeLayer, GColorClear);
  text_layer_set_font(&timeLayer, fonts_get_system_font(FONT_KEY_GOTHAM_34_MEDIUM_NUMBERS));
  text_layer_set_text_alignment(&timeLayer, GTextAlignmentCenter);

  text_layer_init(&speedLayer, GRect(0, 60, 144 /* width */, 168-70 /* height */));
  text_layer_set_text_color(&speedLayer, GColorWhite);
  text_layer_set_background_color(&speedLayer, GColorClear);
  text_layer_set_font(&speedLayer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));

  text_layer_init(&weatherLayer, GRect(0, 94, 144 /* width */, 168-100 /* height */));
  text_layer_set_text_color(&weatherLayer, GColorWhite);
  text_layer_set_background_color(&weatherLayer, GColorClear);
  text_layer_set_font(&weatherLayer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));

  text_layer_init(&dateLayer, GRect(0, 130, 144 /* width */, 168-130 /* height */));
  text_layer_set_text_color(&dateLayer, GColorWhite);
  text_layer_set_background_color(&dateLayer, GColorClear);
  text_layer_set_font(&dateLayer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(&dateLayer, GTextAlignmentCenter);


  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  handle_minute_tick(ctx, NULL);

  layer_add_child(&window.layer, &timeLayer.layer);
  layer_add_child(&window.layer, &dateLayer.layer);
  layer_add_child(&window.layer, &speedLayer.layer);
  layer_add_child(&window.layer, &weatherLayer.layer);

  Tuplet initial_values[] = {
    TupletCString(SPEED_KEY, buffers[SPEED_KEY]),
    TupletCString(WIND_SPEED_KEY, buffers[WIND_SPEED_KEY]),
  };
  app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values, ARRAY_LENGTH(initial_values),
                sync_tuple_changed_callback, sync_error_callback, NULL);
}

 static void handle_deinit(AppContextRef c) {
   app_sync_deinit(&sync);
 }


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit,
    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    },
    .messaging_info = {
      .buffer_sizes = {
        .inbound = 64,
        .outbound = 16
      }
    }
  };
  app_event_loop(params, &handlers);
}
