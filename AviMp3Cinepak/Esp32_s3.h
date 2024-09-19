/*******************************************************************************
 * Start of Arduino_GFX setting
 *
 * Arduino_GFX try to find the settings depends on selected board in Arduino IDE
 * Or you can define the display dev kit not in the board list
 * Defalult pin list for non display dev kit:
 * ESP32 various dev board     : CS:  5, DC: 27, RST: 33, BL: 22, SCK: 18, MOSI: 23, MISO: nil
 * ESP32-C3 various dev board  : CS:  7, DC:  2, RST:  1, BL:  3, SCK:  4, MOSI:  6, MISO: nil
 * ESP32-S2 various dev board  : CS: 34, DC: 38, RST: 33, BL: 21, SCK: 36, MOSI: 35, MISO: nil
 * ESP32-S3 various dev board  : CS: 40, DC: 41, RST: 42, BL: 48, SCK: 36, MOSI: 35, MISO: nil
 ******************************************************************************/
#include <Arduino_GFX_Library.h>

#define GFX_BL DF_GFX_BL // default backlight pin, you may replace DF_GFX_BL to actual backlight pin
// #define GFX_BL 1

/* More dev device declaration: https://github.com/moononournation/Arduino_GFX/wiki/Dev-Device-Declaration */
#if defined(DISPLAY_DEV_KIT)
Arduino_GFX *gfx = create_default_Arduino_GFX();
#else /* !defined(DISPLAY_DEV_KIT) */

/* More data bus class: https://github.com/moononournation/Arduino_GFX/wiki/Data-Bus-Class */
Arduino_DataBus *bus = create_default_Arduino_DataBus();

/* More display class: https://github.com/moononournation/Arduino_GFX/wiki/Display-Class */
// Arduino_GFX *gfx = new Arduino_ILI9341(bus, DF_GFX_RST, 3 /* rotation */, false /* IPS */);
Arduino_GFX *gfx = new Arduino_ST7789(bus, 42 /* RST */, 0 /* rotation */, true /* IPS */, 172, 320, 35, 0, 35, 0);
#endif /* !defined(DISPLAY_DEV_KIT) */
#define GFX_SPEED 80000000UL

// I2C
#define I2C_SDA 18
#define I2C_SCL 8
#define I2C_FREQ 800000UL

#define SD_SCK   12 
#define SD_MOSI  11
#define SD_MISO  13
#define SD_CS    10

// I2S
#define I2S_DEFAULT_GAIN_LEVEL 0.5
#define I2S_OUTPUT_NUM I2S_NUM_0
#define I2S_MCLK -1
#define I2S_BCLK 46
#define I2S_LRCK 2
#define I2S_DOUT 45
#define I2S_DIN -1

// SPI.begin(12 /* CLK */, 13 /* D0/MISO */, 11 /* CMD/MOSI */);
//   if (!SD.begin(10 /* CS */, SPI, 80000000L, ROOT))