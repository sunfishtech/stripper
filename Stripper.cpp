#include <Stripper.h>

Stripper::Stripper(Adafruit_NeoPixel *s, uint16_t r, uint16_t c) : Adafruit_GFX(r, c){
  strip = s;
  rows = r;
  cols = c;
  triggerTime = 0L;
  brightnessTriggerTime = 0L;
  colorTriggerTime = 0L;
  currentColor = 0;
  currentPixel = 0;
  currentWait = 0;
  currentStep = 0;
  currentBrightness = 255;
  targetBrightness = 255;
  brightnessWait = 50;
  fadeAmount = 0;
  loop = false;
  randomColor = false;
  currentCommand = NONE;
  colorMode = STATIC;
  colorWait = 0;
  colorStep = 0;
  currentSize = 0;
  fill = false;
}

void Stripper::reset(StripperCommand cmd){
  if (currentCommand != cmd){
    schedule(NONE,0);
    currentPixel = 0;
    currentColor = 0;
    currentStep = 0;
    targetBrightness = currentBrightness;
    brightnessWait = 50;
    fadeAmount = 0;
    loop = false;
    currentSize = 0;
    fill = false;
  }
}

void Stripper::setColorMode(ColorMode mode, uint16_t wait){
  colorMode = mode;
  colorWait = wait;
}

void Stripper::setPixel(uint16_t pixelNumber, byte *rgb, bool draw) {
  //reset();
  uint32_t color = strip->Color(rgb[0],rgb[1],rgb[2]);
  strip->setPixelColor(pixelNumber-1, color);
  if (draw) strip->show();
}

void Stripper::setPixel(uint16_t c, uint16_t r, byte *rgb, bool draw){
  if (c <= cols && r <= rows){
    uint16_t x = cols - c + 1;
    uint16_t y = rows - r + 1;
    uint16_t pixelNum = (y*cols) - (x-1);
    //uint16_t pn = (pixelNum % strip->numPixels()); #this wraps the pixel
    setPixel(pixelNum, rgb, draw);
  }
}

void Stripper::setPixel(uint16_t x, uint16_t y, uint32_t color, bool draw){
  byte
    r = (byte)(color >> 16),
    g = (byte)(color >> 8),
    b = (byte)color;
  byte rgb[3] = {r,g,b};
  setPixel(x, y, rgb, draw);
}

void Stripper::drawPixel(int16_t x, int16_t y, uint16_t color) {
  uint32_t rgb888 = rgb565to888(color);
  setPixel(x, y, rgb888);
}

void Stripper::rectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t c888, bool fill){
  uint16_t c565 = rgb888to565(c888);
  if (fill)
    fillRect(x, y, w, h, c565);
  else
    drawRect(x, y, w, h, c565);
}

void Stripper::rectangle(int16_t x, int16_t y, int16_t w, int16_t h, byte *rgb, bool fill){
  uint32_t c888 = strip->Color(rgb[0],rgb[1],rgb[2]);
  rectangle(x,y,w,h,c888,fill);
}

void Stripper::setColor(byte *rgb){
 // reset();
  uint16_t i;
  setCurrentColor(rgb);
  for (i=0;i<strip->numPixels();i++){
    strip->setPixelColor(i,getCurrentColor());
  }
  strip->show();
}

void Stripper::off(void){
  reset();
  for(uint16_t i=0;i<strip->numPixels();i++){
    strip->setPixelColor(i,0);
  }
  strip->show();
}

void Stripper::colorWipe(byte *rgb, uint16_t wait) {
  reset(COLOR_WIPE);
  setCurrentColor(rgb);
  currentPixel = 0;
  Stripper::schedule(COLOR_WIPE,wait);
}

uint16_t Stripper::colorWipe(){
  strip->setPixelColor(currentPixel++,getCurrentColor());
  return currentPixel < strip->numPixels() ? currentWait : 0;
}

void Stripper::strobe(byte *rgb, uint16_t wait, uint8_t fadeAmount, uint16_t fadeWait){
  reset(STROBE);
  loop = true;
  setCurrentColor(rgb);
  fade(fadeAmount, fadeWait);
  schedule(STROBE, wait);
}

uint16_t Stripper::strobe(){
  uint16_t i;
  if (currentStep < 1){
    uint32_t color = getCurrentColor();
    for (i=0;i<strip->numPixels();i++){
      strip->setPixelColor(i,color);
    }
  }
  return nextStep(2);
}

void Stripper::rainbow(uint16_t wait, bool l) {
  reset(RAINBOW);
  loop = l;
  Stripper::schedule(RAINBOW,wait);
}

uint16_t Stripper::rainbow(){
  uint16_t i;
  for (i = 0; i < strip->numPixels();i++){
    strip->setPixelColor(i,wheel((i+currentStep) & 255));
  }
  return nextStep(256);
}

// Slightly different, this makes the rainbow equally distributed throughout
void Stripper::rainbowCycle(uint16_t wait, bool l) {
  reset(RAINBOW_CYCLE);
  loop = l;
  Stripper::schedule(RAINBOW_CYCLE, wait);
}

uint16_t Stripper::rainbowCycle(){
  uint16_t i;
  for(i = 0; i< strip->numPixels(); i++){
    strip->setPixelColor(i, wheel(((i * 256 / strip->numPixels()) + currentStep) & 255));
  }
  return nextStep(256*5);
}

void Stripper::colorCycle(uint16_t wait, bool l){
  reset(COLOR_CYCLE);
  loop = l;
  Stripper::schedule(COLOR_CYCLE, wait);
}

uint16_t Stripper::colorCycle(){
  uint16_t i;
  uint32_t color = wheel(currentStep);
  for(i = 0; i < strip->numPixels(); i++){
    strip->setPixelColor(i, color);
  }
  return nextStep(255);
}

void Stripper::scan(byte *rgb, uint16_t wait, uint8_t fadeAmount, uint16_t fadeWait){
  reset(SCAN);
  loop = true;
  setCurrentColor(rgb);
  fade(fadeAmount, fadeWait);
  schedule(SCAN, wait);
}

uint16_t Stripper::scan(){
  uint16_t prev = currentStep == 0 ? strip->numPixels()-1 : currentStep-1;
  if (fadeAmount == 0){
    strip->setPixelColor(prev, 0);
  }
  strip->setPixelColor(currentStep, getCurrentColor());
  return nextStep(strip->numPixels());
}

void Stripper::rain(byte *rgb, uint16_t wait, uint8_t fadeAmount, uint16_t fadeWait){
  reset(RAIN);
  setCurrentColor(rgb);
  fade(fadeAmount,fadeWait);
  schedule(RAIN,wait);
}

uint16_t Stripper::rain(){
  if (fadeAmount == 0) {
    strip->setPixelColor(currentPixel,0);
  }
  currentPixel = (uint16_t)random(0,strip->numPixels());
  strip->setPixelColor(currentPixel,getCurrentColor());
  return currentWait;
}

void Stripper::squares(byte *rgb, uint16_t size, bool f, uint16_t wait, uint8_t fadeAmount, uint16_t fadeWait){
  reset(SQUARES);
  setCurrentColor(rgb);
  fade(fadeAmount,fadeWait);
  currentSize = size;
  fill = f;
  schedule(SQUARES,wait);
}

uint16_t Stripper::squares(){
  int16_t x = (int16_t)random(1,cols);
  int16_t y = (int16_t)random(1,rows);
  int16_t w = currentSize > 0 ? currentSize : random(1,5);
  rectangle(x,y,w,w,getCurrentColor(), fill);
  return currentWait;
}


void Stripper::fade(uint8_t amount, uint16_t wait, bool l){
  fadeAmount = amount;
  targetBrightness = currentBrightness;
  brightnessWait = wait;
  loop = l;
  if (loop)
    schedule(FADE,wait);
}

uint16_t Stripper::fade(){
  return currentWait;
}

void Stripper::brightness(uint8_t bright) {
  currentBrightness = bright;
  strip->setBrightness(bright);
  strip->show();
}

void Stripper::fadeStrip(){
  for (uint16_t i = 0; i<strip->numPixels();i++){
    uint32_t c = strip->getPixelColor(i);
    uint8_t
      r = (uint8_t)(c >> 16),
      g = (uint8_t)(c >>  8),
      b = (uint8_t)c;

    r = r > fadeAmount ? r - fadeAmount : 0;
    g = g > fadeAmount ? g - fadeAmount : 0;
    b = b > fadeAmount ? b - fadeAmount : 0;

    c = strip->Color(r,g,b);
    strip->setPixelColor(i,c);
  }
}



// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Stripper::wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip->Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip->Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip->Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void Stripper::setCurrentColor(byte *rgb){
  currentColor = strip->Color(rgb[0],rgb[1],rgb[2]);
}

uint32_t Stripper::getCurrentColor(){
  return currentColor;
}

void Stripper::schedule(StripperCommand cmd, uint16_t wait) {
  if (wait > 0){
    triggerTime = millis() + (uint32_t)wait;
    currentCommand = cmd;
    currentWait = wait;
  } else {
    triggerTime = 0;
    currentCommand = NONE;
    currentWait = 0;
  }
}

bool Stripper::tick() {
  bool redraw = false;
  redraw = checkBrightness() || redraw;
  redraw = checkFade() || redraw;
  redraw = checkColor() || redraw;
  if (currentCommand != NONE && triggerTime <= millis()){
    schedule(currentCommand, executeNextCommand());
    strip->show();
    return true;
  } else {
    if (redraw)
      strip->show();
    return false;
  }
}

uint16_t Stripper::executeNextCommand() {
  switch (currentCommand){
    case COLOR_WIPE:
      return colorWipe(); break;
    case RAINBOW:
      return rainbow(); break;
    case RAINBOW_CYCLE:
      return rainbowCycle(); break;
    case COLOR_CYCLE:
      return colorCycle(); break;
    case SCAN:
      return scan(); break;
    case RAIN:
      return rain(); break;
    case STROBE:
      return strobe(); break;
    case SQUARES:
      return squares(); break;
    case FADE:
      return fade(); break;
    deault:
      return 0;
  };
}

bool Stripper::checkBrightness(){
  bool changed = false;
  if (targetBrightness != currentBrightness &&
      brightnessTriggerTime < millis()){
    changed = true;
    if (currentBrightness < targetBrightness){
      strip->setBrightness(++currentBrightness);
    } else if (currentBrightness > targetBrightness) {
      strip->setBrightness(--currentBrightness);
    }
    brightnessTriggerTime = millis() + (uint32_t)brightnessWait;
  }
  return changed;
}

bool Stripper::checkFade() {
  bool changed = false;
  if (fadeAmount > 0 && brightnessTriggerTime < millis()){
    changed = true;
    fadeStrip();
    brightnessTriggerTime = millis() + (uint32_t)brightnessWait;
  }
  return changed;
}

bool Stripper::checkColor() {
  bool changed = false;
  if (colorMode != STATIC && colorTriggerTime < millis()){
    changed = true;
    if (colorMode == RANDOM) {
      currentColor = wheel((byte)random(0,256));
    }
    else if (colorMode == WHEEL) {
      colorStep = ++colorStep;
      if (colorStep > 255)
        colorStep = 0;
      currentColor = wheel(colorStep);
    }
    colorTriggerTime = millis() + (uint32_t)colorWait;
  }
  return changed;
}

uint16_t Stripper::nextStep(uint16_t max){
  currentStep++;
  if (currentStep >= max){
    if (loop){
      currentStep = 0;
    } else {
      return 0;
    }
  }
  return currentWait;
}

uint16_t Stripper::rgb888to565(uint32_t rgb888){
  byte
    r = (byte)(rgb888 >> 16),
    g = (byte)(rgb888 >> 8),
    b = (byte)rgb888;

  byte
    R5 = ( r * 249 + 1014 ) >> 11,
    G6 = ( g * 253 +  505 ) >> 10,
    B5 = ( b * 249 + 1014 ) >> 11;

  return ((uint16_t)R5 << 11) | ((uint16_t)G6 << 5) | B5;
}

uint32_t Stripper::rgb565to888(uint16_t rgb565){
  byte
    r = (byte)(rgb565 >> 11),
    g = (byte)((rgb565 >> 5) & 63),
    b = (byte)(rgb565 & 31);
  byte
    R8 = ( r * 527 + 23 ) >> 6,
    G8 = ( g * 259 + 33 ) >> 6,
    B8 = ( b * 527 + 23 ) >> 6;

  return strip->Color(R8,G8,B8);
}





