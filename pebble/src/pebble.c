#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0xED, 0xB5, 0x02, 0xF4, 0xEC, 0xB4, 0x4C, 0x6E, 0x9E, 0xD6, 0x80, 0x71, 0x8C, 0x28, 0xC7, 0x98 }
PBL_APP_INFO(MY_UUID,
             "Sailor Watch", "Sasha Ognev",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;

AppSync sync;
uint8_t sync_buffer[32];

TextLayer timeLayer; // The clock
TextLayer dateLayer;
TextLayer speedLayer;
TextLayer windLayer;

enum {
  SPEED_KEY = 0x0,
  WIND_SPEED_KEY = 0x1,
  GUST_SPEED_KEY = 0x2,
  WIND_DIRECTION_KEY = 0x3
};

char* buffers[] = {"Speed: 000.00 kts", "Wind 000.00 kts", "Gust 000.00 kts", "From 000%"};
static char knots[] = "kts";

// Called once per minute
void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {

  (void)t;
  (void)ctx;

  static char timeText[] = "00:00"; // Needs to be static because it's used by the system later.

  PblTm currentTime;

  get_time(&currentTime);
  string_format_time(timeText, sizeof(timeText), "%R", &currentTime);
  text_layer_set_text(&timeLayer, timeText);
}

static char* itoa2(char* buffer, int num)
{
    int temp = num;
    int len = 0;
    while(temp >= 1)
    {
        temp /= 10;
        ++len;
    }
    if (len == 0)
    {
        len = 1;
    }
    for (int i = 0; i < len; ++i)
    {
        buffer[(len-1)-i] = '0' + (num % 10);
        num /= 10;
    }
    return buffer + len;
}

static char* splice_num(char* buffer, double num, int precision)
{
    buffer = itoa2(buffer, (int)num);
    if (precision > 0)
    {
        *buffer = '.';
        ++buffer;
    }
    double fractional = (num - (int)(num));
    for (int i = 0; i < precision; ++i)
    {
        fractional *= 10;
    }
    return itoa2(buffer, (int)fractional);
}

static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  (void) dict_error;
  (void) app_message_error;
  (void) context;
  // TODO: HANDLE ERRORS
}

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
  (void) old_tuple;
  double speed;
  char* pch;

  switch (key) {
  case SPEED_KEY:
    speed = (double)new_tuple->value->uint32 / 100.0;
    pch = splice_num(buffers[key] + 7, speed, 1);
    *pch = ' ';
    strcpy(pch+1, knots);
    text_layer_set_text(&speedLayer, buffers[key]);
    break;
  case WIND_SPEED_KEY:
  case GUST_SPEED_KEY:
  case WIND_DIRECTION_KEY:
    break;
  default:
    return;
  }
}

void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Ahoy");
  window_stack_push(&window, true /* Animated */);

  window_set_background_color(&window, GColorBlack);

  text_layer_init(&timeLayer, GRect(40, 44, 144-40 /* width */, 168-44 /* height */));
  text_layer_set_text_color(&timeLayer, GColorWhite);
  text_layer_set_background_color(&timeLayer, GColorClear);
  text_layer_set_font(&timeLayer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));

  text_layer_init(&speedLayer, GRect(40, 70, 144-40 /* width */, 168-70 /* height */));
  text_layer_set_text_color(&speedLayer, GColorWhite);
  text_layer_set_background_color(&speedLayer, GColorClear);
  text_layer_set_font(&speedLayer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));


  text_layer_init(&dateLayer, GRect(40, 130, 144-40 /* width */, 168-130 /* height */));
  text_layer_set_text_color(&dateLayer, GColorWhite);
  text_layer_set_background_color(&dateLayer, GColorClear);
  text_layer_set_font(&dateLayer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));


  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  handle_minute_tick(ctx, NULL);

  layer_add_child(&window.layer, &timeLayer.layer);
  layer_add_child(&window.layer, &dateLayer.layer);
  layer_add_child(&window.layer, &speedLayer.layer);

  Tuplet initial_values[] = {
    TupletInteger(SPEED_KEY, (int32_t) 0),
    TupletInteger(WIND_SPEED_KEY, (int32_t) 0),
    TupletInteger(GUST_SPEED_KEY, (int32_t) 0),
    TupletInteger(WIND_DIRECTION_KEY, (int32_t) 0),
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
