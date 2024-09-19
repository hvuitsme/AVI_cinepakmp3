/*******************************************************************************
 * AVI Player example
 *
 * Dependent libraries:
 * Arduino_GFX: https://github.com/moononournation/Arduino_GFX.git
 * avilib: https://github.com/lanyou1900/avilib.git
 * libhelix: https://github.com/pschatzmann/arduino-libhelix.git
 *
 * Setup steps:
 * 1. Change your LCD parameters in Arduino_GFX setting
 * 2. Upload AVI file
 *   FFat/LittleFS:
 *     upload FFat (FatFS) data with ESP32 Sketch Data Upload:
 *     ESP32: https://github.com/lorol/arduino-esp32fs-plugin
 *   SD:
 *     Copy files to SD card
 ******************************************************************************/

//  command: ffmpeg -i cat.mp4 -vf "fps=30,scale=-1:320:flags=lanczos,crop=172:in_h:(in_w-172)/2:0" -c:v cinepak -q:v 7 cat_30fps_cinepak.avi
//           * ffmpeg -y -i input.mp4 -ac 2 -ar 44100 -af loudnorm -c:a mp3 -c:v cinepak -q:v 7 -vf "fps=30,scale=-1:320:flags=lanczos,crop=172:in_h:(in_w-172)/2:0" mashle_30fps_cinepak.avi
//           ffmpeg -i cat.mp4 -c:a mp3 -c:v cinepak -q:v 7 -vf "fps=30,scale=172:320:flags=lanczos" cat_30fps_cinepak.avi
//           and Due to the specific requirement of this codec standard that dimensions must be multiples of 4, the width must be 172 if the screen width is 170.

const char *root = "/root";
char *avi_filename = (char *)"/root/shiroko_30fps_cinepak.avi";
// char *avi_filename = (char *)"/root/AviMp3Cinepak272p30fps.avi";

// #include "T_DECK.h"
// #include "JC4827W543.h"
#include "Esp32_s3.h"

#include <FFat.h>
#include <LittleFS.h>
#include <SPIFFS.h>
#include <SD.h>
#include <SD_MMC.h>

size_t output_buf_size;
uint16_t *output_buf;

#include "AviFunc.h"
#include "esp32_audio.h"

void setup()
{
  Serial.begin(115200);
  // Serial.setDebugOutput(true);
  // while(!Serial);
  Serial.println("AviPcmu8Mjpeg");

  // If display and SD shared same interface, init SPI first
#ifdef SPI_SCK
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
#endif

#ifdef GFX_EXTRA_PRE_INIT
  GFX_EXTRA_PRE_INIT();
#endif

  // Init Display
  // if (!gfx->begin())
  if (!gfx->begin(GFX_SPEED))
  {
    Serial.println("gfx->begin() failed!");
  }
  gfx->fillScreen(BLACK);

#ifdef GFX_BL
// #if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR < 3)
//   ledcSetup(0, 1000, 8);
//   ledcAttachPin(GFX_BL, 0);
//   ledcWrite(0, 204);
// #else  // ESP_ARDUINO_VERSION_MAJOR >= 3
//   ledcAttachChannel(GFX_BL, 1000, 8, 1);
//   ledcWrite(GFX_BL, 204);
// #endif // ESP_ARDUINO_VERSION_MAJOR >= 3

  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
#endif // GFX_BL

  // gfx->setTextColor(WHITE, BLACK);
  // gfx->setTextBound(60, 60, 240, 240);

#ifdef AUDIO_MUTE_PIN
  pinMode(AUDIO_MUTE_PIN, OUTPUT);
  digitalWrite(AUDIO_MUTE_PIN, HIGH);
#endif

  i2s_init();

#if defined(SD_D1)
  SD_MMC.setPins(SD_SCK, SD_MOSI /* CMD */, SD_MISO /* D0 */, SD_D1, SD_D2, SD_CS /* D3 */);
  if (!SD_MMC.begin(root, false /* mode1bit */, false /* format_if_mount_failed */, SDMMC_FREQ_HIGHSPEED))
#elif defined(SD_SCK)
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  SD_MMC.setPins(SD_SCK, SD_MOSI /* CMD */, SD_MISO /* D0 */);
  if (!SD_MMC.begin(root, true /* mode1bit */, false /* format_if_mount_failed */, SDMMC_FREQ_DEFAULT))
#elif defined(SD_CS)
  if (!SD.begin(SD_CS, SPI, 80000000, "/root"))
#else
  // if (!FFat.begin(false, root))
  // if (!LittleFS.begin(false, root))
  // if (!SPIFFS.begin(false, root))
#endif
  {
    Serial.println("ERROR: File system mount failed!");
  }
  else
  {
    output_buf_size = gfx->width() * gfx->height() * 2;
#ifdef RGB_PANEL
    output_buf = gfx->getFramebuffer();
#else
    output_buf = (uint16_t *)aligned_alloc(16, output_buf_size);
#endif
    if (!output_buf)
    {
      Serial.println("output_buf aligned_alloc failed!");
    }

    avi_init();
  }
}

void loop()
{
  if (avi_open(avi_filename))
  {
    Serial.println("AVI start");
    gfx->fillScreen(BLACK);

    i2s_set_sample_rate(avi_aRate);

    avi_feed_audio();

    Serial.println("Start play audio task");
    BaseType_t ret_val = mp3_player_task_start();
    if (ret_val != pdPASS)
    {
      Serial.printf("mp3_player_task_start failed: %d\n", ret_val);
    }

    avi_start_ms = millis();

    Serial.println("Start play loop");
    while (avi_curr_frame < avi_total_frames)
    {
      avi_feed_audio();
      if (avi_decode())
      {
        avi_draw(0, 0);
      }
    }

    avi_close();
    Serial.println("AVI end");

    // avi_show_stat();
  }

  // delay(60 * 1000);
}
