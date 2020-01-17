#include <TFT_eSPI.h>
#include <WiFi.h>
#include <stdlib.h>
#include <JPEGDecoder.h>
#include <pgmspace.h>

#ifndef TFT_DISPOFF
#define TFT_DISPOFF 0x28
#endif

#ifndef TFT_SLPIN
#define TFT_SLPIN   0x10
#endif

#define minimum(a,b)     (((a) < (b)) ? (a) : (b))
#define TFT_MOSI            19
#define TFT_SCLK            18
#define TFT_CS              5
#define TFT_DC              16
#define TFT_RST             23

TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library

const char* ssid     = "tnu";
const char* password = "tnu20170201000";
const char* host = "shard-mars.glitch.me";
uint8_t image[18480] PROGMEM={0};
void setup()
{
    tft.init();
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);
    setwifi();
}



void setwifi()
{
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    delay(5000);
}


void loop()
{
        show_image();
        delay(10000);
}
void show_image()
{
  WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
      delay(4000);
      show_image();
    }
    String url = "/";
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
            client.stop();
        }
    }
    String line;
    int a=0;
    while(client.available()) {
        line = client.readStringUntil('\r');
    }
     char delimiter[] = ",";
    char* ptr = strtok(&line[1], delimiter);
    while(ptr != NULL) {
        image[a] = (int) strtol( ptr, NULL, 16);
        a++;
        ptr = strtok(NULL, delimiter);
    }
    drawArrayJpeg(image, sizeof(image), 0, 0);
}




















void drawArrayJpeg(const uint8_t arrayname[], uint32_t array_size, int xpos, int ypos) {
  int x = xpos;
  int y = ypos;
  JpegDec.decodeArray(arrayname, array_size);
  renderJPEG(x, y);
}
void renderJPEG(int xpos, int ypos) {
  uint16_t *pImg;
  uint16_t mcu_w = JpegDec.MCUWidth;
  uint16_t mcu_h = JpegDec.MCUHeight;
  uint32_t max_x = JpegDec.width;
  uint32_t max_y = JpegDec.height;
  uint32_t min_w = minimum(mcu_w, max_x % mcu_w);
  uint32_t min_h = minimum(mcu_h, max_y % mcu_h);
  uint32_t win_w = mcu_w;
  uint32_t win_h = mcu_h;
  max_x += xpos;
  max_y += ypos;
  while (JpegDec.readSwappedBytes()) {
    pImg = JpegDec.pImage ;
    int mcu_x = JpegDec.MCUx * mcu_w + xpos;  // Calculate coordinates of top left corner of current MCU
    int mcu_y = JpegDec.MCUy * mcu_h + ypos;
    if (mcu_x + mcu_w <= max_x) win_w = mcu_w;
    else win_w = min_w;
    if (mcu_y + mcu_h <= max_y) win_h = mcu_h;
    else win_h = min_h;
    if (win_w != mcu_w)
    {
      uint16_t *cImg;
      int p = 0;
      cImg = pImg + win_w;
      for (int h = 1; h < win_h; h++)
      {
        p += mcu_w;
        for (int w = 0; w < win_w; w++)
        {
          *cImg = *(pImg + w + p);
          cImg++;
        }
      }
    }
    if (( mcu_x + win_w ) <= tft.width() && ( mcu_y + win_h ) <= tft.height())
    {
      tft.pushRect(mcu_x, mcu_y, win_w, win_h, pImg);
    }
    else if ( (mcu_y + win_h) >= tft.height()) JpegDec.abort();
  }
}
