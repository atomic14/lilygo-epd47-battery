#include <epd_driver.h>
#include <epd_highlevel.h>
#include <esp_sleep.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>
#include "regular_font.h"
#include <string>

extern "C"
{
  void app_main();
}

void app_main()
{
  epd_init(EPD_OPTIONS_DEFAULT);
  epd_poweron();
  auto font_props = epd_font_properties_default();
  // fallback to a question mark for character not available in the font
  font_props.fallback_glyph = '?';
  // start up the EPD
  epd_init(EPD_OPTIONS_DEFAULT);
  auto hl = epd_hl_init(EPD_BUILTIN_WAVEFORM);
  // first set full screen to white
  epd_set_rotation(EPD_ROT_INVERTED_PORTRAIT);
  auto frame_buffer = epd_hl_get_framebuffer(&hl);
  epd_hl_set_all_white(&hl);
  epd_fullclear(&hl, 20);
  // read the battery from gpio 36 ADC1 channel 0
  // and display the result
  adc1_config_width(ADC_WIDTH_12Bit);
  adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
  // get the adc calibration
  esp_adc_cal_characteristics_t adc_chars;
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
  auto adc_value = adc1_get_raw(ADC1_CHANNEL_0);
  // get the actual calibrated voltage
  auto voltage = 2 * esp_adc_cal_raw_to_voltage(adc_value, &adc_chars);
  auto text1 = "ADC Reading: " + std::to_string(adc_value);
  auto text2 = "Voltage: " + std::to_string(voltage) + "mV";
  int x = 20;
  int y = 50;
  epd_write_string(&regular_font, text1.c_str(), &x, &y, frame_buffer, &font_props);
  x = 20;
  epd_write_string(&regular_font, text2.c_str(), &x, &y, frame_buffer, &font_props);
  epd_hl_update_screen(&hl, MODE_GC16, 20);
  // power off an go into deep sleep for 30 seconds
  epd_poweroff();
  esp_sleep_enable_timer_wakeup(30000000);
  esp_deep_sleep_start();
}