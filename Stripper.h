#ifndef _Stripper
#define _Stripper

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>

typedef enum StripperCommand {
  NONE = 0,
  COLOR_WIPE = 1,
  RAINBOW = 2,
  RAINBOW_CYCLE = 3,
  COLOR_CYCLE = 4,
  SCAN = 5,
  FADE = 6,
  RAIN = 7,
  STROBE = 8,
  SQUARES = 9
};

typedef enum ColorMode {
  STATIC = 0,
  RANDOM = 1,
  WHEEL = 2
};

class Stripper : public Adafruit_GFX {
  public:
    Stripper(Adafruit_NeoPixel *strip, uint16_t rows, uint16_t cols);

  void
    setPixel(uint16_t pixelNumber, byte *c, bool draw=true),
    setPixel(uint16_t x, uint16_t y, byte *c, bool draw=true),  
    setPixel(uint16_t x, uint16_t y, uint32_t color, bool draw=true),
    setColor(byte *c),
    colorWipe(byte *c, uint16_t wait),
    rainbow(uint16_t wait, bool loop = true),
    rainbowCycle(uint16_t wait, bool loop = true),
    brightness(uint8_t bright),
    colorCycle(uint16_t wait, bool loop = true),
    scan(byte *c, uint16_t wait, uint8_t fadeAmount = 10, uint16_t fadeWait = 50),
    rain(byte *c, uint16_t wait, uint8_t fadeAmount = 50, uint16_t fadeWait = 50),
    squares(byte *c, uint16_t size, bool fill, uint16_t wait, uint8_t fadeAmount=10, uint16_t fadeWait=50),
    fade(uint8_t amount, uint16_t wait, bool loop=false),
    strobe(byte *c, uint16_t wait, uint8_t fadeAmount, uint16_t fadeWait),
    off(void),
    setColorMode(ColorMode mode, uint16_t wait),
    drawPixel(int16_t x, int16_t y, uint16_t color),
    rectangle(int16_t x, int16_t y, int16_t w, int16_t h, byte* color, bool fill=false),
    rectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color, bool fill=false);
    bool tick();


  protected:
    uint32_t
      wheel(unsigned char wheelPosition),
      getCurrentColor(),
      rgb565to888(uint16_t rgb565);
    void
      schedule(StripperCommand cmd, uint16_t wait),
      reset(StripperCommand cmd = NONE),
      fadeStrip(),
      setCurrentColor(byte *rgb);
    bool
      checkBrightness(),
      checkFade(),
      checkColor();
    uint16_t
      colorWipe(),
      rainbow(),
      rainbowCycle(),
      colorCycle(),
      scan(),
      rain(),
      squares(),
      strobe(),
      fade(),
      executeNextCommand(),
      nextStep(uint16_t max),
      rgb888to565(uint32_t rgb888);

  private:
    Adafruit_NeoPixel *strip;
    uint32_t
      triggerTime,
      brightnessTriggerTime,
      colorTriggerTime,
      currentColor;
    uint16_t
      currentPixel,
      currentWait,
      currentStep,
      colorWait,
      rows, cols;
    uint8_t
      currentBrightness,
      brightnessWait,
      targetBrightness,
      fadeAmount,
      colorStep,
      currentSize;
    bool loop,
         randomColor,
         fill;
    StripperCommand currentCommand;
    ColorMode colorMode;
};

#endif
