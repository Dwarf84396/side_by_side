#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0x26, 0x25, 0x9E, 0x9C, 0xEB, 0xC2, 0x44, 0x5F, 0x8E, 0xEB, 0x39, 0x0C, 0xD1, 0xF1, 0xCF, 0x4C }
PBL_APP_INFO(MY_UUID,
             "Side By Side", "Ben Froman",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

void upcase(char *str)
{
    for (int i = 0; str[i] != 0; i++) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] -= 0x20;
        }
    }
}

Window window;

TextLayer text_hours_layer;
TextLayer text_minutes_layer;
TextLayer text_seconds_layer;
TextLayer text_year_layer;
TextLayer text_month_layer;
TextLayer text_date_layer;
TextLayer text_day_layer;

Layer centerLine_layer;
Layer boundingBoxes_layer;

void handle_second_tick (AppContextRef ctx, PebbleTickEvent *t);

void graphics_draw_rect (GContext* ctx, GRect rect) {
    graphics_draw_line(ctx, GPoint(rect.origin.x, rect.origin.y), GPoint(rect.origin.x, rect.origin.y + rect.size.h));
    graphics_draw_line(ctx, GPoint(rect.origin.x, rect.origin.y + rect.size.h), GPoint(rect.origin.x + rect.size.w, rect.origin.y + rect.size.h));
    graphics_draw_line(ctx, GPoint(rect.origin.x + rect.size.w, rect.origin.y + rect.size.h), GPoint(rect.origin.x + rect.size.w, rect.origin.y));
    graphics_draw_line(ctx, GPoint(rect.origin.x + rect.size.w, rect.origin.y), GPoint(rect.origin.x, rect.origin.y));
}

void centerLine_layer_update_callback (Layer *me, GContext* ctx) {
    (void)me;
    
    graphics_context_set_stroke_color(ctx, GColorWhite);
    
    graphics_draw_line(ctx, GPoint(71, 8), GPoint(71, 160));
    graphics_draw_line(ctx, GPoint(72, 8), GPoint(72, 160));
    
    // Hour
    graphics_draw_line(ctx, GPoint(64,14), GPoint(64,20));
    graphics_draw_line(ctx, GPoint(68,14), GPoint(68,20));
    graphics_draw_line(ctx, GPoint(64,17), GPoint(68,17));
    
    // Minute
    graphics_draw_line(ctx, GPoint(64,59), GPoint(64,65));
    graphics_draw_line(ctx, GPoint(68,59), GPoint(68,65));
    graphics_draw_line(ctx, GPoint(64,59), GPoint(66,61));
    graphics_draw_line(ctx, GPoint(66,61), GPoint(68,59));
    
    // Second
    graphics_draw_line(ctx, GPoint(67,104), GPoint(68,105));
    graphics_draw_line(ctx, GPoint(67,104), GPoint(65,104));
    graphics_draw_line(ctx, GPoint(65,104), GPoint(64,105));
    graphics_draw_line(ctx, GPoint(64,105), GPoint(64,106));
    graphics_draw_line(ctx, GPoint(64,106), GPoint(65,107));
    graphics_draw_line(ctx, GPoint(65,107), GPoint(67,107));
    graphics_draw_line(ctx, GPoint(67,107), GPoint(68,108));
    graphics_draw_line(ctx, GPoint(68,108), GPoint(68,109));
    graphics_draw_line(ctx, GPoint(68,109), GPoint(67,110));
    graphics_draw_line(ctx, GPoint(67,110), GPoint(65,110));
    graphics_draw_line(ctx, GPoint(65,110), GPoint(64,109));
    
    // Year
    graphics_draw_line(ctx, GPoint(75,8), GPoint(77,10));
    graphics_draw_line(ctx, GPoint(77,10), GPoint(79,8));
    graphics_draw_line(ctx, GPoint(77,10), GPoint(77,14));
    
    // Month
    graphics_draw_line(ctx, GPoint(75,50), GPoint(75,56));
    graphics_draw_line(ctx, GPoint(79,50), GPoint(79,56));
    graphics_draw_line(ctx, GPoint(75,50), GPoint(77,52));
    graphics_draw_line(ctx, GPoint(77,52), GPoint(79,50));
    
    // Day
    graphics_draw_line(ctx, GPoint(75,92), GPoint(75,98));
    graphics_draw_line(ctx, GPoint(75,92), GPoint(77,92));
    graphics_draw_line(ctx, GPoint(77,92), GPoint(79,94));
    graphics_draw_line(ctx, GPoint(79,94), GPoint(79,96));
    graphics_draw_line(ctx, GPoint(79,96), GPoint(77,98));
    graphics_draw_line(ctx, GPoint(77,98), GPoint(75,98));
    
    
}

void boundingBoxes_layer_update_callback (Layer *me, GContext* ctx) {
    (void)me;
    
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_context_set_fill_color(ctx, GColorBlack);
    
    graphics_draw_rect(ctx, text_hours_layer.layer.frame);
    graphics_draw_rect(ctx, text_minutes_layer.layer.frame);
    graphics_draw_rect(ctx, text_seconds_layer.layer.frame);
    graphics_draw_rect(ctx, text_year_layer.layer.frame);
    graphics_draw_rect(ctx, text_month_layer.layer.frame);
    graphics_draw_rect(ctx, text_date_layer.layer.frame);
    graphics_draw_rect(ctx, text_day_layer.layer.frame);
}

void handle_init(AppContextRef ctx) {
    window_init(&window, "Side By Side");
    window_stack_push(&window, true /* Animated */);
    window_set_background_color(&window, GColorBlack);
    
    resource_init_current_app(&APP_RESOURCES);
    
    //layer_init(&boundingBoxes_layer, window.layer.frame);
    //boundingBoxes_layer.update_proc = &boundingBoxes_layer_update_callback;
    //layer_add_child(&window.layer, &boundingBoxes_layer);
    
    text_layer_init(&text_hours_layer, window.layer.frame);
    text_layer_set_text_color(&text_hours_layer, GColorWhite);
    text_layer_set_background_color(&text_hours_layer, GColorClear);
    layer_set_frame(&text_hours_layer.layer, GRect(4, 14, 144-84, 41));
    text_layer_set_font(&text_hours_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CST_BLACK_34)));
    layer_add_child(&window.layer, &text_hours_layer.layer);
    
    text_layer_init(&text_minutes_layer, window.layer.frame);
    text_layer_set_text_color(&text_minutes_layer, GColorWhite);
    text_layer_set_background_color(&text_minutes_layer, GColorClear);
    layer_set_frame(&text_minutes_layer.layer, GRect(4, 59, 144-84, 41));
    text_layer_set_font(&text_minutes_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CST_BLACK_34)));
    layer_add_child(&window.layer, &text_minutes_layer.layer);
    
    text_layer_init(&text_seconds_layer, window.layer.frame);
    text_layer_set_text_color(&text_seconds_layer, GColorWhite);
    text_layer_set_background_color(&text_seconds_layer, GColorClear);
    layer_set_frame(&text_seconds_layer.layer, GRect(4, 104, 144-84, 41));
    text_layer_set_font(&text_seconds_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CST_BLACK_34)));
    layer_add_child(&window.layer, &text_seconds_layer.layer);
    
    text_layer_init(&text_year_layer, window.layer.frame);
    text_layer_set_text_color(&text_year_layer, GColorWhite);
    text_layer_set_background_color(&text_year_layer, GColorClear);
    layer_set_frame(&text_year_layer.layer, GRect(78, 8, 144-86, 37));
    text_layer_set_font(&text_year_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CST_LIGHT_30)));
    layer_add_child(&window.layer, &text_year_layer.layer);
    
    text_layer_init(&text_month_layer, window.layer.frame);
    text_layer_set_text_color(&text_month_layer, GColorWhite);
    text_layer_set_background_color(&text_month_layer, GColorClear);
    layer_set_frame(&text_month_layer.layer, GRect(78, 50, 144-86, 37));
    text_layer_set_font(&text_month_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CST_LIGHT_30)));
    layer_add_child(&window.layer, &text_month_layer.layer);
    
    text_layer_init(&text_date_layer, window.layer.frame);
    text_layer_set_text_color(&text_date_layer, GColorWhite);
    text_layer_set_background_color(&text_date_layer, GColorClear);
    layer_set_frame(&text_date_layer.layer, GRect(78, 92, 144-86, 37));
    text_layer_set_font(&text_date_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CST_LIGHT_30)));
    layer_add_child(&window.layer, &text_date_layer.layer);
    
    text_layer_init(&text_day_layer, window.layer.frame);
    text_layer_set_text_color(&text_day_layer, GColorWhite);
    text_layer_set_background_color(&text_day_layer, GColorClear);
    layer_set_frame(&text_day_layer.layer, GRect(78, 134, 144-86, 27));
    text_layer_set_font(&text_day_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CST_LIGHT_20)));
    layer_add_child(&window.layer, &text_day_layer.layer);
    
    layer_init(&centerLine_layer, window.layer.frame);
    centerLine_layer.update_proc = &centerLine_layer_update_callback;
    layer_add_child(&window.layer, &centerLine_layer);
    
    handle_second_tick (ctx, GCornerNone);
}

void handle_second_tick (AppContextRef ctx, PebbleTickEvent *t) {
    (void)ctx;
    
    // Need to be static because they're used by the system later.
    static char hours_text[] = "77";
    static char minutes_text[] = "77";
    static char seconds_text[] = "77";
    static char year_text[] = "00";
    static char month_text[] = "00";
    static char date_text[] = "00";
    static char day_text[] = "WED";
    
    PblTm tm;
    
    if (t == GCornerNone) {
        get_time(&tm);
    } else {
        tm = *(t->tick_time);
    }
    
    // TODO: Only update the date when it's changed.
    if (clock_is_24h_style()) {
        string_format_time(hours_text, sizeof(hours_text), "%H", &tm);
    } else {
        string_format_time(hours_text, sizeof(hours_text), "%I", &tm);
    }
    text_layer_set_text(&text_hours_layer, hours_text);
    
    string_format_time(minutes_text, sizeof(minutes_text), "%M", &tm);
    text_layer_set_text(&text_minutes_layer, minutes_text);
    
    string_format_time(seconds_text, sizeof(seconds_text), "%S", &tm);
    text_layer_set_text(&text_seconds_layer, seconds_text);
    
    string_format_time(year_text, sizeof(year_text), "%y", &tm);
    text_layer_set_text(&text_year_layer, year_text);
    
    string_format_time(month_text, sizeof(month_text), "%m", &tm);
    text_layer_set_text(&text_month_layer, month_text);
    
    string_format_time(date_text, sizeof(date_text), "%e", &tm);
    text_layer_set_text(&text_date_layer, date_text);
    
    string_format_time(day_text, sizeof(day_text), "%a", &tm);
    upcase((char*)(&day_text));
    text_layer_set_text(&text_day_layer, day_text);
}


void pbl_main(void *params) {
    PebbleAppHandlers handlers = {
        .init_handler = &handle_init,
      
        .tick_info = {
            .tick_handler = &handle_second_tick,
            .tick_units = SECOND_UNIT
        }
    };
    
    app_event_loop(params, &handlers);
}
