#include <Stripper.h>
#include <Adafruit_NeoPixel.h>


Stripper::Stripper(Adafruit_NeoPixel *s){
  strip = s;
  triggerTime = 0L;
  brightnessTriggerTime = 0L;
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
}

void Stripper::reset(){
  schedule(NONE,0);
  currentPixel = 0;
  currentColor = 0;
  currentStep = 0;
  targetBrightness = currentBrightness;
  brightnessWait = 50;
  fadeAmount = 0;
  loop = false;
}

void Stripper::setPixel(uint16_t pixelNumber, byte *rgb) {
  reset();
  uint32_t color = strip->Color(rgb[0],rgb[1],rgb[2]);
  strip->setPixelColor(pixelNumber-1, color);
  strip->show();
}

void Stripper::setColor(byte *rgb){
  reset();
  uint16_t i;
  uint32_t color = strip->Color(rgb[0],rgb[1],rgb[2]);
  for (i=0;i<strip->numPixels();i++){
    strip->setPixelColor(i,color);
  }
  strip->show();
}

void Stripper::off(void){
  reset();
  byte rgb[3] = {0,0,0};
  setColor(rgb);
}

void Stripper::colorWipe(byte *rgb, uint16_t wait) {
  reset();
  setCurrentColor(rgb);
  currentPixel = 0;
  Stripper::schedule(COLOR_WIPE,wait);
}

uint16_t Stripper::colorWipe(){
  strip->setPixelColor(currentPixel++,getCurrentColor());
  return currentPixel < strip->numPixels() ? currentWait : 0;
}

void Stripper::rainbow(uint16_t wait, bool l) {
  reset();
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
  reset();
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
  reset();
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
  reset();
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
  reset();
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

void Stripper::dim(uint8_t brightness, uint8_t wait){
  targetBrightness = brightness;
  brightnessWait = wait;
}

void Stripper::fade(uint8_t amount, uint16_t wait){
  fadeAmount = amount;
  targetBrightness = currentBrightness;
  brightnessWait = wait;
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

bool Stripper::isRandomColor(byte *rgb){
  return (rgb[0] + rgb[1] + rgb[2]) == 0;
}

void Stripper::setCurrentColor(byte *rgb){
  randomColor = isRandomColor(rgb);
  currentColor = strip->Color(rgb[0],rgb[1],rgb[2]);
}

uint32_t Stripper::getCurrentColor(){
  if (randomColor)
    currentColor = wheel((byte)random(0,256));
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





