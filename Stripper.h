#ifndef _Stripper
#define _Stripper

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <Adafruit_NeoPixel.h>

typedef enum StripperCommand {
  NONE = 0,
  COLOR_WIPE = 1,
  RAINBOW = 2,
  RAINBOW_CYCLE = 3,
  COLOR_CYCLE = 4,
  SCAN = 5,
  DIM = 6,
  RAIN = 7
};

class Stripper {
  public:
    Stripper(Adafruit_NeoPixel *strip, uint16_t rows, uint16_t cols);

  void
    setPixel(uint16_t pixelNumber, byte *c, bool draw=true),
    setPixel(uint16_t x, uint16_t y, byte *c, bool draw=true),  
    setColor(byte *c),
    colorWipe(byte *c, uint16_t wait),
    rainbow(uint16_t wait, bool loop = true),
    rainbowCycle(uint16_t wait, bool loop = true),
    brightness(uint8_t bright),
    colorCycle(uint16_t wait, bool loop = true),
    scan(byte *c, uint16_t wait, uint8_t fadeAmount = 10, uint16_t fadeWait = 50),
    rain(byte *c, uint16_t wait, uint8_t fadeAmount = 50, uint16_t fadeWait = 50),
    dim(uint8_t brightness, uint8_t wait),
    fade(uint8_t amount, uint16_t wait),
    off(void);

    bool tick();

  protected:
    uint32_t
      wheel(unsigned char wheelPosition),
      getCurrentColor();
    void
      schedule(StripperCommand cmd, uint16_t wait),
      reset(),
      fadeStrip(),
      setCurrentColor(byte *rgb);
    bool
      checkBrightness(),
      checkFade(),
      isRandomColor(byte *rgb);
    uint16_t
      colorWipe(),
      rainbow(),
      rainbowCycle(),
      colorCycle(),
      scan(),
      rain(),
      executeNextCommand(),
      nextStep(uint16_t max);

  private:
    Adafruit_NeoPixel *strip;
    uint32_t
      triggerTime,
      brightnessTriggerTime,
      currentColor;
    uint16_t
      currentPixel,
      currentWait,
      currentStep,
      rows, cols;
    uint8_t
      currentBrightness,
      brightnessWait,
      targetBrightness,
      fadeAmount;
    bool loop,
         randomColor;
    StripperCommand currentCommand;

};



#endif
