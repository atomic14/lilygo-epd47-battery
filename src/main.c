#include <epd_driver.h>
#include <epd_highlevel.h>
#include "regular_font.h"

void app_main()
{
  epd_init();
  epd_power_on();
  auto font_props = epd_font_properties_default();
  // fallback to a question mark for character not available in the font
  font_props.fallback_glyph = '?';
  // start up the EPD
  epd_init(EPD_OPTIONS_DEFAULT);
  auto hl = epd_hl_init(EPD_BUILTIN_WAVEFORM);
  // first set full screen to white
  epd_hl_set_all_white(&hl);
  epd_set_rotation(EPD_ROT_INVERTED_PORTRAIT);
  auto frame_buffer = epd_hl_get_framebuffer(&m_hl);
  // read the battery from gpio 36 ADC1 channel 0
  // and display the result
  auto adc_value = adc1_get_raw(ADC1_CHANNEL_0);
  auto voltage = adc_value * 3.3 / 4095;
  auto text = "Battery: " + std::to_string(voltage) + "V";
  epd_hl_draw_text(&hl, frame_buffer, text.c_str(), font_props, 0, 0);
  epd_hl_update_screen(&m_hl, MODE_GC16, 20);
  // power off an go to sleep
  epd_power_off();
  esp_deep_sleep_start();
}