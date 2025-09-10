#include "../core/options.h"
#if DSP_MODEL == DSP_SSD1322

#include "displaySSD1322.h"
#include "../core/config.h"
#include "../core/network.h"

#define LOGO_WIDTH 21  //21
#define LOGO_HEIGHT 32


#ifndef DEF_SPI_FREQ
#define DEF_SPI_FREQ 16000000UL /*  set it to 0 for system default */
#endif
#ifndef SSD1322_GRAYSCALE
#define SSD1322_GRAYSCALE false
#endif
const unsigned char logo[] PROGMEM = {
  // 'logo, 
   0x00, 0x62, 0x00,  // Anteny
    0x00, 0x62, 0x00,
    0x00, 0x62, 0x00,
    0x00, 0x62, 0x00,
    0x00, 0x62, 0x00,
    0x00, 0x7E, 0x00,  // Początek wieży
    0x00, 0x7E, 0x00,
    0x00, 0x7E, 0x00,
    0x00, 0x7E, 0x00,
    0x00, 0x7E, 0x00,
    0x00, 0x7E, 0x00,
    0x00, 0x7E, 0x00,
    0x00, 0x7E, 0x00,
    0x00, 0x7E, 0x00,
    0x00, 0x7E, 0x00,
    0x00, 0x7E, 0x00,
    0x00, 0x7E, 0x00,
    0x00, 0x7E, 0x00,
    0x00, 0x7E, 0x00,
    0x00, 0x7E, 0x00,
    0x00, 0x7E, 0x00,
    0x00, 0x7E, 0x00,
    0x00, 0x7E, 0x00,
    0x00, 0x7E, 0x00,
    0x00, 0x7E, 0x00,
    0x00, 0xFE, 0x00,  // Podstawa
    0x01, 0xFE, 0x00,
    0x03, 0xFE, 0x00,
    0x07, 0xFE, 0x00,
    0x0F, 0xFE, 0x00,
    0x1F, 0xFE, 0x00,
    0x3F, 0xFE, 0x00
};


#if DSP_HSPI
DspCore::DspCore()
  : Jamis_SSD1322(256, 64, &SPI2, TFT_DC, TFT_RST, TFT_CS, DEF_SPI_FREQ) {}
#else
DspCore::DspCore()
  : Jamis_SSD1322(256, 64, &SPI, TFT_DC, TFT_RST, TFT_CS, DEF_SPI_FREQ) {}
#endif

#include "tools/utf8RusGFX.h"

void DspCore::initDisplay() {
#if !SSD1322_GRAYSCALE
#include "tools/oledcolorfix.h"
#else
  config.theme.background = TFT_BG;
#if DSP_INVERT_TITLE
  config.theme.meta = TFT_FG;
  config.theme.metabg = TFT_BG;
  config.theme.metafill = TFT_BG;
  config.theme.clock = TFT_FG;  //white czcionka
  config.theme.clockbg = TFT_BG;
#else
  config.theme.meta = TFT_BG;
  config.theme.metabg = GRAY_9;
  config.theme.metafill = GRAY_9;
  config.theme.clock = TFT_BG;  //white czcionka
  config.theme.clockbg = TFT_FG;
#endif
  config.theme.weather = GRAY_3;
  config.theme.title1 = GRAY_9;
  config.theme.title2 = GRAY_3;
  config.theme.rssi = GRAY_5;
  config.theme.ip = GRAY_3;
  config.theme.vol = GRAY_9;
  config.theme.bitrate = GRAY_7;
  config.theme.digit = GRAY_9;
  config.theme.buffer = TFT_FG;
  config.theme.volbarout = GRAY_9;
  config.theme.volbarin = GRAY_9;
  config.theme.plcurrent = TFT_BG;      //-------literki
  config.theme.plcurrentbg = TFT_FG;    //-------tło literek
  config.theme.plcurrentfill = TFT_FG;  //-------tłopod napisem

  for (byte i = 0; i < 5; i++) config.theme.playlist[i] = GRAY_9;//-------pozostale napisy playlisty
#endif  //!SSD1322_GRAYSCALE

  begin();
  cp437(true);
  flip();
  invert();
  setTextWrap(false);

  plItemHeight = playlistConf.widget.textsize * (CHARHEIGHT - 1) + playlistConf.widget.textsize * 4;
  plTtemsCount = round((float)height() / plItemHeight);
  if (plTtemsCount % 2 == 0) plTtemsCount++;
  plCurrentPos = plTtemsCount / 2;
  plYStart = (height() / 2 - plItemHeight / 2) - plItemHeight * (plTtemsCount - 1) / 2 + playlistConf.widget.textsize * 2;
}

void DspCore::drawLogo(uint16_t top) {
  for (int x = 0; x <10; x++) {
    drawBitmap((width() - LOGO_WIDTH) / 2, 10, logo, LOGO_WIDTH, LOGO_HEIGHT, x);
    display();
    delay(200);
  }
}

void DspCore::printPLitem(uint8_t pos, const char* item, ScrollWidget& current) {
  setTextSize(playlistConf.widget.textsize);
  if (pos == plCurrentPos) {
    current.setText(item);
  } else {
    uint8_t plColor = (abs(pos - plCurrentPos) - 1) > 4 ? 4 : abs(pos - plCurrentPos) - 1;
    setTextColor(config.theme.playlist[plColor], config.theme.background);
    setCursor(TFT_FRAMEWDT, plYStart + pos * plItemHeight);
    fillRect(0, plYStart + pos * plItemHeight - 1, width(), plItemHeight - 2, config.theme.background);
    print(utf8Rus(item, true));
  }
}

void DspCore::drawPlaylist(uint16_t currentItem) {
  uint8_t lastPos = config.fillPlMenu(currentItem - plCurrentPos, plTtemsCount);
  if (lastPos < plTtemsCount) {
    fillRect(0, lastPos * plItemHeight + plYStart, width(), height() / 2, config.theme.background);
  }
}

void DspCore::clearDsp(bool black) {
  //fillScreen(TFT_BG);
  clearDisplay();
}

GFXglyph* pgm_read_glyph_ptr(const GFXfont* gfxFont, uint8_t c) {
  return gfxFont->glyph + c;
}
//------------------------funkcja wyliczania wielkosci czcionki -----------------------
uint8_t DspCore::_charWidth(unsigned char c) {
  GFXglyph* glyph = pgm_read_glyph_ptr(&Robo14pt7b, c - 0x20);
  return pgm_read_byte(&glyph->xAdvance);
}

uint16_t DspCore::textWidth(const char* txt) {
  uint16_t w = 0, l = strlen(txt);
  for (uint16_t c = 0; c < l; c++) w += _charWidth(txt[c]);
  return w;
}

void DspCore::_getTimeBounds() {
  _timewidth = textWidth(_timeBuf);
  char buf[4];
  strftime(buf, 4, "%H", &network.timeinfo);
  _dotsLeft = textWidth(buf);
}

void DspCore::_clockSeconds() {

  setTextSize(1);

  setFont(&Robo14pt7b);

  setTextColor((network.timeinfo.tm_sec % 2 == 0) ? config.theme.clock : (CLOCKFONT_MONO ? config.theme.clockbg : config.theme.background), config.theme.background);
  setCursor(_timeleft + _dotsLeft, clockTop);
  print(":");

  setFont();
}

void DspCore::_clockDate() {
  //if(_olddateleft>0) dsp.fillRect(_olddateleft,  clockTop-28, _olddatewidth+8, CHARHEIGHT, config.theme.background); //poprawka 24.05
  if (_olddateleft > 0) dsp.fillRect(_olddateleft, clockTop - 24, _olddatewidth + 8, CHARHEIGHT, config.theme.background);
  setTextColor(GRAY_9, TFT_BG);
  setCursor(_dateleft, clockTop - 24);        //24 tu regulujesz polozenie daty top
  if (!config.isScreensaver) print(_dateBuf); /* print date */
  strlcpy(_oldDateBuf, _dateBuf, sizeof(_dateBuf));
  _olddatewidth = _datewidth;
  _olddateleft = _dateleft;
}

void DspCore::_clockTime() {
  if (_oldtimeleft > 0 && !CLOCKFONT_MONO) dsp.fillRect(_oldtimeleft, clockTop - clockTimeHeight, _oldtimewidth + CHARWIDTH * 2 + 2, clockTimeHeight, config.theme.background);
  _timeleft = (width() /*/2*/ - _timewidth /*/2*/) - clockRightSpace;

  setTextSize(1);

  setFont(&Robo14pt7b);

  if (CLOCKFONT_MONO) {
    setCursor(_timeleft, clockTop);
    setTextColor(config.theme.clockbg, config.theme.background);
    print("88:88");
  }
  clearClock();
  setCursor(_timeleft, clockTop);
  setTextColor(TFT_FG, GRAY_3);
  //setTextColor(config.theme.clock, config.theme.background);
  print(_timeBuf);
  setFont();

  strlcpy(_oldTimeBuf, _timeBuf, sizeof(_timeBuf));
  _oldtimewidth = _timewidth;
  _oldtimeleft = _timeleft;

  sprintf(_buffordate, "%02d.%s.%d", network.timeinfo.tm_mday, mnths[network.timeinfo.tm_mon], network.timeinfo.tm_year + 1900);  //tu drukowanie daty
  strlcpy(_dateBuf, utf8Rus(_buffordate, true), sizeof(_dateBuf));
  _datewidth = strlen(_dateBuf) * CHARWIDTH;
  _dateleft = width() - clockRightSpace - _datewidth;
}

void DspCore::printClock(uint16_t top, uint16_t rightspace, uint16_t timeheight, bool redraw) {
  clockTop = top;
  clockRightSpace = rightspace;
  clockTimeHeight = timeheight;
  strftime(_timeBuf, sizeof(_timeBuf), "%H:%M", &network.timeinfo);
  if (strcmp(_oldTimeBuf, _timeBuf) != 0 || redraw) {
    _getTimeBounds();
    _clockTime();
    if (!config.isScreensaver)
      if (strcmp(_oldDateBuf, _dateBuf) != 0 || redraw) _clockDate();
  }
  _clockSeconds();
}

void DspCore::clearClock() {
  dsp.fillRect(_timeleft, clockTop - clockTimeHeight, _timewidth + 12, clockTimeHeight + 1, config.theme.background);
}

void DspCore::startWrite(void) {
  //TAKE_MUTEX();
  Jamis_SSD1322::startWrite();
}

void DspCore::endWrite(void) {
  Jamis_SSD1322::endWrite();
  //GIVE_MUTEX();
}

void DspCore::loop(bool force) {
  display();
  delay(5);
}

void DspCore::charSize(uint8_t textsize, uint8_t& width, uint16_t& height) {
  width = textsize * CHARWIDTH;
  height = textsize * CHARHEIGHT;
}

void DspCore::setTextSize(uint8_t s) {
  Adafruit_GFX::setTextSize(s);
}

void DspCore::flip() {
  setRotation(config.store.flipscreen ? 2 : 0);
}

void DspCore::invert() {
  invertDisplay(config.store.invertdisplay);
}

void DspCore::sleep(void) {
  oled_command(SSD1322_DISPLAYOFF);
}
void DspCore::wake(void) {
  oled_command(SSD1322_DISPLAYON);
}

void DspCore::writePixel(int16_t x, int16_t y, uint16_t color) {
  if (_clipping) {
    if ((x < _cliparea.left) || (x > _cliparea.left + _cliparea.width) || (y < _cliparea.top) || (y > _cliparea.top + _cliparea.height)) return;
  }
  Jamis_SSD1322::writePixel(x, y, color);
}

void DspCore::writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  if (_clipping) {
    if ((x < _cliparea.left) || (x >= _cliparea.left + _cliparea.width) || (y < _cliparea.top) || (y > _cliparea.top + _cliparea.height)) return;
  }
  Jamis_SSD1322::writeFillRect(x, y, w, h, color);
}

void DspCore::setClipping(clipArea ca) {
  _cliparea = ca;
  _clipping = true;
}

void DspCore::clearClipping() {
  _clipping = false;
}

void DspCore::setNumFont() {
  setFont(&Robo14pt7b);
  setTextSize(1);
}

#endif
