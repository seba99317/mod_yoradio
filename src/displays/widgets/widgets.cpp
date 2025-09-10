#include "arduinoFFT.h"
#include "../dspcore.h"
#if DSP_MODEL != DSP_DUMMY

#include "widgets.h"
#include "../../core/player.h"  //  for VU widget
#include "../../displays/fonts/icons.h"
#include "../../displays/fonts/TinyFont5.h"
#include "../../displays/fonts/TinyFont3.h"
/************************
      FILL WIDGET
 ************************/
void FillWidget::init(FillConfig conf, uint16_t bgcolor) {
  Widget::init(conf.widget, bgcolor, bgcolor);
  _width = conf.width;
  _height = conf.height;
}

void FillWidget::_draw() {
  if (!_active) return;
  dsp.fillRect(_config.left, _config.top, _width, _height, _bgcolor);
}

void FillWidget::setHeight(uint16_t newHeight) {
  _height = newHeight;
  //_draw();
}
/************************
      TEXT WIDGET
 ************************/
TextWidget::~TextWidget() {
  free(_text);
  free(_oldtext);
}

void TextWidget::init(WidgetConfig wconf, uint16_t buffsize, bool uppercase, uint16_t fgcolor, uint16_t bgcolor) {
  Widget::init(wconf, fgcolor, bgcolor);
  _buffsize = buffsize;
  _text = (char *)malloc(sizeof(char) * _buffsize);
  memset(_text, 0, _buffsize);
  _oldtext = (char *)malloc(sizeof(char) * _buffsize);
  memset(_oldtext, 0, _buffsize);
  //_charWidth = wconf.textsize * CHARWIDTH;    // default GFX font
  //_textheight = wconf.textsize * CHARHEIGHT;   // default GFX font
  dsp.charSize(_config.textsize, _charWidth, _textheight);
  _textwidth = _oldtextwidth = _oldleft = 0;
  _uppercase = uppercase;
}

void TextWidget::setText(const char *txt) {
  strlcpy(_text, dsp.utf8Rus(txt, _uppercase), _buffsize);
  _textwidth = strlen(_text) * _charWidth;
  if (strcmp(_oldtext, _text) == 0) return;
  if (_active) dsp.fillRect(_oldleft == 0 ? _realLeft() : min(_oldleft, _realLeft()), _config.top, max(_oldtextwidth, _textwidth), _textheight, _bgcolor);
  _oldtextwidth = _textwidth;
  _oldleft = _realLeft();
  if (_active) _draw();
}

void TextWidget::setText(int val, const char *format) {
  char buf[_buffsize];
  snprintf(buf, _buffsize, format, val);
  setText(buf);
}

void TextWidget::setText(const char *txt, const char *format) {
  char buf[_buffsize];
  snprintf(buf, _buffsize, format, txt);
  setText(buf);
}

uint16_t TextWidget::_realLeft() {
  switch (_config.align) {
    case WA_CENTER: return (dsp.width() - _textwidth) / 2; break;
    case WA_RIGHT: return (dsp.width() - _textwidth - _config.left); break;
    default: return _config.left; break;
  }
}

void TextWidget::_draw() {
  if (!_active) return;
  dsp.setTextColor(_fgcolor, _bgcolor);
  dsp.setCursor(_realLeft(), _config.top);
  //dsp.setFont();//------------------------------------------ustawienie fontu standard

  dsp.setTextSize(_config.textsize);
  dsp.print(_text);
  strlcpy(_oldtext, _text, _buffsize);
}

/************************
      SCROLL WIDGET
 ************************/
ScrollWidget::ScrollWidget(const char *separator, ScrollConfig conf, uint16_t fgcolor, uint16_t bgcolor) {
  init(separator, conf, fgcolor, bgcolor);
}

ScrollWidget::~ScrollWidget() {
  free(_sep);
  free(_window);
}

void ScrollWidget::init(const char *separator, ScrollConfig conf, uint16_t fgcolor, uint16_t bgcolor) {
  TextWidget::init(conf.widget, conf.buffsize, conf.uppercase, fgcolor, bgcolor);
  _sep = (char *)malloc(sizeof(char) * 4);
  memset(_sep, 0, 4);
  snprintf(_sep, 4, " %.*s ", 1, separator);
  _x = conf.widget.left;
  _startscrolldelay = conf.startscrolldelay;
  _scrolldelta = conf.scrolldelta;
  _scrolltime = conf.scrolltime;
  //_charWidth = CHARWIDTH * _config.textsize;           // default GFX font
  //_textheight = CHARHEIGHT * _config.textsize;          // default GFX font
  dsp.charSize(_config.textsize, _charWidth, _textheight);
  _sepwidth = strlen(_sep) * _charWidth;
  _width = conf.width;
  _backMove.width = _width;
  _window = (char *)malloc(sizeof(char) * (MAX_WIDTH / _charWidth + 1));
  memset(_window, 0, (MAX_WIDTH / _charWidth + 1));  // +1?
  _doscroll = false;
}

void ScrollWidget::_setTextParams() {
  if (_config.textsize == 0) return;
  dsp.setTextSize(_config.textsize);
  dsp.setTextColor(_fgcolor, _bgcolor);
}

bool ScrollWidget::_checkIsScrollNeeded() {
  return _textwidth > _width;
}

void ScrollWidget::setText(const char *txt) {
  strlcpy(_text, dsp.utf8Rus(txt, _uppercase), _buffsize - 1);
  if (strcmp(_oldtext, _text) == 0) return;
  _textwidth = strlen(_text) * _charWidth;
  _x = _config.left;
  _doscroll = _checkIsScrollNeeded();
  if (dsp.getScrollId() == this) dsp.setScrollId(NULL);
  _scrolldelay = millis();
  if (_active) {
    _setTextParams();
    if (_doscroll) {
      dsp.fillRect(_config.left, _config.top, _width, _textheight, _bgcolor);
      dsp.setCursor(_config.left, _config.top);
      snprintf(_window, _width / _charWidth + 1, "%s", _text);  //TODO
      dsp.setClipping({ _config.left, _config.top, _width, _textheight });
      dsp.print(_window);
      dsp.clearClipping();
    } else {
      dsp.fillRect(_config.left, _config.top, _width, _textheight, _bgcolor);
      dsp.setCursor(_realLeft(), _config.top);
      dsp.print(_text);
    }
    strlcpy(_oldtext, _text, _buffsize);
  }
}

void ScrollWidget::setText(const char *txt, const char *format) {
  char buf[_buffsize];
  snprintf(buf, _buffsize, format, txt);
  setText(buf);
}

void ScrollWidget::loop() {
  if (_locked) return;
  if (!_doscroll || _config.textsize == 0 || (dsp.getScrollId() != NULL && dsp.getScrollId() != this)) return;
  if (_checkDelay(_x == _config.left ? _startscrolldelay : _scrolltime, _scrolldelay)) {
    _calcX();
    if (_active) _draw();
  }
}

void ScrollWidget::_clear() {
  dsp.fillRect(_config.left, _config.top, _width, _textheight, _bgcolor);
}

void ScrollWidget::_draw() {
  if (!_active || _locked) return;
  _setTextParams();
  if (_doscroll) {
    uint16_t _newx = _config.left - _x;
    const char *_cursor = _text + _newx / _charWidth;
    uint16_t hiddenChars = _cursor - _text;
    if (hiddenChars < strlen(_text)) {
      snprintf(_window, _width / _charWidth + 1, "%s%s%s", _cursor, _sep, _text);
    } else {
      const char *_scursor = _sep + (_cursor - (_text + strlen(_text)));
      snprintf(_window, _width / _charWidth + 1, "%s%s", _scursor, _text);
    }
    dsp.setCursor(_x + hiddenChars * _charWidth, _config.top);
    dsp.setClipping({ _config.left, _config.top, _width, _textheight });
    dsp.print(_window);
#ifndef DSP_LCD
    dsp.print(" ");
#endif
    dsp.clearClipping();
  } else {
    dsp.fillRect(_config.left, _config.top, _width, _textheight, _bgcolor);
    dsp.setCursor(_realLeft(), _config.top);
    dsp.setClipping({ _realLeft(), _config.top, _width, _textheight });
    dsp.print(_text);
    dsp.clearClipping();
  }
}

void ScrollWidget::_calcX() {
  if (!_doscroll || _config.textsize == 0) return;
  _x -= _scrolldelta;
  if (-_x > _textwidth + _sepwidth - _config.left) {
    _x = _config.left;
    dsp.setScrollId(NULL);
  } else {
    dsp.setScrollId(this);
  }
}

bool ScrollWidget::_checkDelay(int m, uint32_t &tstamp) {
  if (millis() - tstamp > m) {
    tstamp = millis();
    return true;
  } else {
    return false;
  }
}

void ScrollWidget::_reset() {
  dsp.setScrollId(NULL);
  _x = _config.left;
  _scrolldelay = millis();
  _doscroll = _checkIsScrollNeeded();
}

/******************************************
      SLIDER WIDGET PASEK GLOSCOSCI
 ******************************************/
void SliderWidget::init(FillConfig conf, uint16_t fgcolor, uint16_t bgcolor, uint32_t maxval, uint16_t oucolor) {
  Widget::init(conf.widget, fgcolor, bgcolor);
  _width = conf.width;
  _height = conf.height;
  _outlined = conf.outlined;
  _oucolor = oucolor, _max = maxval;
  _oldvalwidth = _value = 0;
}

void SliderWidget::setValue(uint32_t val) {
  _value = val;
  if (_active && !_locked) _drawslider();
}

void SliderWidget::_drawslider() {  //rysowanie aktualnych zmian volume
  uint16_t valwidth = map(_value, 0, _max, 0, _width - _outlined * 2);
  if (_oldvalwidth == valwidth) return;
  dsp.fillRect(_config.left + _outlined + min(valwidth, _oldvalwidth), _config.top + _outlined, abs(_oldvalwidth - valwidth), _height - _outlined * 2, _oldvalwidth > valwidth ? _bgcolor : GRAY_5);
  _oldvalwidth = valwidth;
}

void SliderWidget::_draw() {  //rysowanie starych wskazan volume
  if (_locked) return;
  _clear();
  if (!_active) return;
  if (_outlined) dsp.drawRect(_config.left, _config.top, _width, _height, TFT_FG);  // ustawienia rysowania outline
  uint16_t valwidth = map(_value, 0, _max, 0, _width - _outlined * 2);
  dsp.fillRect(_config.left + _outlined, _config.top + _outlined, valwidth, _height - _outlined * 2, GRAY_5);
}

void SliderWidget::_clear() {  //czyszczenie wskazan volume
                               //  _oldvalwidth = 0;
  dsp.fillRect(_config.left, _config.top, _width, _height, _bgcolor);
}
void SliderWidget::_reset() {
  _oldvalwidth = 0;
}
/************************
      VU WIDGET
 ************************/
#if !defined(DSP_LCD) && !defined(DSP_OLED)
VuWidget::~VuWidget() {
  if (_canvas) free(_canvas);
}

void VuWidget::init(WidgetConfig wconf, VUBandsConfig bands, uint16_t vumaxcolor, uint16_t vumincolor, uint16_t bgcolor) {
  Widget::init(wconf, bgcolor, bgcolor);
  _vumaxcolor = vumaxcolor;
  _vumincolor = vumincolor;
  _bands = bands;
  _canvas = new Canvas(_bands.width * 2 + _bands.space, _bands.height);
}

void VuWidget::_draw() {
  _clear();
  if (!_active || _locked) return;
#if !defined(USE_NEXTION) && I2S_DOUT == 255
/*  static uint8_t cc = 0;
  cc++;
  if(cc>0){
    player.getVUlevel();
    cc=0;
  }*/
#endif
  static uint16_t measL, measR;
  uint16_t bandColor;
  uint16_t dimension = _config.align ? _bands.width : _bands.height;
  uint16_t vulevel = player.get_VUlevel(dimension);

  uint8_t L = (vulevel >> 8) & 0xFF;
  uint8_t R = vulevel & 0xFF;

  bool played = player.isRunning();
  if (played) {
    measL = (L >= measL) ? measL + _bands.fadespeed : L;
    measR = (R >= measR) ? measR + _bands.fadespeed : R;
  } else {
    if (measL < dimension) measL += _bands.fadespeed;
    if (measR < dimension) measR += _bands.fadespeed;
  }
  if (measL > dimension) measL = dimension;
  if (measR > dimension) measR = dimension;
  uint8_t h = (dimension / _bands.perheight) - _bands.vspace;
  _canvas->fillRect(0, 0, _bands.width * 2 + _bands.space, _bands.height, _bgcolor);
  for (int i = 0; i < dimension; i++) {
    if (i % (dimension / _bands.perheight) == 0) {
      if (_config.align) {
#ifndef BOOMBOX_STYLE
        bandColor = (i > _bands.width - (_bands.width / _bands.perheight) * 4) ? _vumaxcolor : _vumincolor;
        _canvas->fillRect(i, 0, h, _bands.height, bandColor);
        _canvas->fillRect(i + _bands.width + _bands.space, 0, h, _bands.height, bandColor);
#else
        bandColor = (i > (_bands.width / _bands.perheight)) ? _vumincolor : _vumaxcolor;
        _canvas->fillRect(i, 0, h, _bands.height, bandColor);
        bandColor = (i > _bands.width - (_bands.width / _bands.perheight) * 3) ? _vumaxcolor : _vumincolor;
        _canvas->fillRect(i + _bands.width + _bands.space, 0, h, _bands.height, bandColor);
#endif
      } else {
        bandColor = (i < (_bands.height / _bands.perheight) * 3) ? _vumaxcolor : _vumincolor;
        _canvas->fillRect(0, i, _bands.width, h, bandColor);
        _canvas->fillRect(_bands.width + _bands.space, i, _bands.width, h, bandColor);
      }
    }
  }
  if (_config.align) {
#ifndef BOOMBOX_STYLE
    _canvas->fillRect(_bands.width - measL, 0, measL, _bands.width, _bgcolor);
    _canvas->fillRect(_bands.width * 2 + _bands.space - measR, 0, measR, _bands.width, _bgcolor);
    dsp.drawRGBBitmap(_config.left, _config.top, _canvas->getBuffer(), _bands.width * 2 + _bands.space, _bands.height);
#else
    _canvas->fillRect(0, 0, _bands.width - (_bands.width - measL), _bands.width, _bgcolor);
    _canvas->fillRect(_bands.width * 2 + _bands.space - measR, 0, measR, _bands.width, _bgcolor);
    dsp.drawRGBBitmap(_config.left, _config.top, _canvas->getBuffer(), _bands.width * 2 + _bands.space, _bands.height);
#endif
  } else {
    _canvas->fillRect(0, 0, _bands.width, measL, _bgcolor);
    _canvas->fillRect(_bands.width + _bands.space, 0, _bands.width, measR, _bgcolor);
    dsp.drawRGBBitmap(_config.left, _config.top, _canvas->getBuffer(), _bands.width * 2 + _bands.space, _bands.height);
  }
}
void VuWidget::loop() {
  if (_active || !_locked) _draw();
}
void VuWidget::_clear() {
  dsp.fillRect(_config.left, _config.top, _bands.width * 2 + _bands.space, _bands.height, _bgcolor);
}

#else  // DSP_LCD
/******************************************************************/
/************************ OLED SSD 1322 ***************************/
/******************************************************************/

#include <cmath>

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max);

VuWidget::~VuWidget() {}

void VuWidget::init(WidgetConfig wconf, VUBandsConfig bands, uint16_t vumaxcolor, uint16_t vumincolor, uint16_t bgcolor) {
    Widget::init(wconf, bgcolor, bgcolor);
    _vumaxcolor = vumaxcolor;
    _vumincolor = vumincolor;
    _bands = bands;
    _maxDimension = 216;
    _peakL = 0;
    _peakR = 0;
    _peakFallDelayCounter = 0;
}

void VuWidget::_draw() {
    if (!_active || _locked) return;

static uint16_t measL_static = 0;
    static uint16_t measR_static = 0;
    static uint16_t L_smooth = 0;
    static uint16_t R_smooth = 0;
    const float smoothing_factor = 0.1;  // Zmniejszono dla szybszej reakcji
    const uint8_t peakFallRate = 15;     // Zwiększono dla szybszego opadania
    const uint8_t VU_LINEAR_SILENCE_THRESHOLD = 20;  // Zmniejszono dla lepszej czułości
    const uint8_t VU_MIN_LEVEL_THRESHOLD = 2;        // Zmniejszono

    uint16_t bandColor;
    uint16_t vulevel_raw = player.get_VUlevel(_maxDimension);
    uint16_t L_raw = (vulevel_raw >> 8) & 0xFF;
    uint16_t R_raw = vulevel_raw & 0xFF;

    uint16_t L_current_linear_temp = 255 - L_raw;
    uint16_t R_current_linear_temp = 255 - R_raw;

    // Wygładzanie sygnału
    L_smooth = (uint16_t)(smoothing_factor * L_current_linear_temp + (1 - smoothing_factor) * L_smooth);
    R_smooth = (uint16_t)(smoothing_factor * R_current_linear_temp + (1 - smoothing_factor) * R_smooth);

    // Filtracja ciszy i minimalnego poziomu sygnału
    uint16_t L_current_linear = (L_smooth < VU_LINEAR_SILENCE_THRESHOLD) ? 0 : L_smooth;
    uint16_t R_current_linear = (R_smooth < VU_LINEAR_SILENCE_THRESHOLD) ? 0 : R_smooth;

    // Histereza: ignoruj bardzo małe wartości
    if (L_current_linear < VU_MIN_LEVEL_THRESHOLD) L_current_linear = 0;
    if (R_current_linear < VU_MIN_LEVEL_THRESHOLD) R_current_linear = 0;

    uint16_t L_current_scaled = (uint16_t)std::round(mapFloat(L_current_linear, 0.0f, 255.0f, 0.0f, (float)_maxDimension));
    uint16_t R_current_scaled = (uint16_t)std::round(mapFloat(R_current_linear, 0.0f, 255.0f, 0.0f, (float)_maxDimension));

    if (L_current_scaled > _maxDimension) L_current_scaled = _maxDimension;
    if (L_current_scaled < 0) L_current_scaled = 0;
    if (R_current_scaled > _maxDimension) R_current_scaled = _maxDimension;
    if (R_current_scaled < 0) R_current_scaled = 0;

    bool isAboveSilence = (L_current_linear > 0 || R_current_linear > 0);
    if (isAboveSilence) {
        bool peakUpdated = false;
        if (L_current_scaled > _peakL) {
            _peakL = L_current_scaled;
            peakUpdated = true;
        }
        if (R_current_scaled > _peakR) {
            _peakR = R_current_scaled;
            peakUpdated = true;
        }
        if (peakUpdated) {
            _peakFallDelayCounter = 0;
        } else {
            _peakFallDelayCounter++;
        }
    } else {
        _peakFallDelayCounter++;
    }

    if (_peakFallDelayCounter >= 5) {
        if (_peakL > 0) _peakL -= peakFallRate;
        if (_peakR > 0) _peakR -= peakFallRate;
        _peakFallDelayCounter = 0;
    }

    if (_peakL < 0) _peakL = 0;
    if (_peakL > _maxDimension) _peakL = _maxDimension;
    if (_peakR < 0) _peakR = 0;
    if (_peakR > _maxDimension) _peakR = _maxDimension;

    bool played = player.isRunning();
    if (played) {
        measL_static = (L_current_scaled >= measL_static) ? L_current_scaled : measL_static - _bands.fadespeed;
        measR_static = (R_current_scaled >= measR_static) ? R_current_scaled : measR_static - _bands.fadespeed;
    } else {
        if (measL_static > 0) measL_static -= _bands.fadespeed;
        if (measR_static > 0) measR_static -= _bands.fadespeed;
        _peakL = 0;
        _peakR = 0;
        _peakFallDelayCounter = 0;
    }

    if (measL_static < 0) measL_static = 0;
    if (measL_static > _maxDimension) measL_static = _maxDimension;
    if (measR_static < 0) measR_static = 0;
    if (measR_static > _maxDimension) measR_static = _maxDimension;

    uint8_t h = (_maxDimension / _bands.perheight) - _bands.vspace;
    dsp.drawRect(_config.left + 9, _config.top - 2, _maxDimension + 4, 17, GRAY_5);
    dsp.setTextSize(_config.textsize);
    dsp.setFont(&TinyFont5);
    dsp.setTextColor(TFT_FG, _bgcolor);
    dsp.setCursor(_config.left + 1, _config.top + 4);
    dsp.print("L");
    dsp.setCursor(_config.left + 1, _config.top + _bands.space + 6);
    dsp.print("R");
    dsp.setFont(&TinyFont3);
    dsp.setCursor(_config.left + 9, _config.top - 6);
    dsp.print("0%");
    dsp.setCursor(_config.left + 12 + (int)(_maxDimension * 0.25) - 6, _config.top - 6);
    dsp.print("25");
    dsp.setCursor(_config.left + 12 + (int)(_maxDimension * 0.50) - 6, _config.top - 6);
    dsp.print("50");
    dsp.setCursor(_config.left + 12 + (int)(_maxDimension * 0.75) - 6, _config.top - 6);
    dsp.print("75");
    dsp.setCursor(_config.left + 12 + _maxDimension - 10, _config.top - 6);
    dsp.print("100");
    dsp.drawLine(_config.left + 9, _config.top - 2, _config.left + 9, _config.top - 5, GRAY_9);
    dsp.drawLine(_config.left + 9 + (int)(_maxDimension * 0.25), _config.top - 2,
                 _config.left + 9 + (int)(_maxDimension * 0.25), _config.top - 5, GRAY_9);
    dsp.drawLine(_config.left + 9 + (int)(_maxDimension * 0.50), _config.top - 2,
                 _config.left + 9 + (int)(_maxDimension * 0.50), _config.top - 5, GRAY_9);
    dsp.drawLine(_config.left + 9 + (int)(_maxDimension * 0.75), _config.top - 2,
                 _config.left + 9 + (int)(_maxDimension * 0.75), _config.top - 5, GRAY_9);
    dsp.drawLine(_config.left + 12 + _maxDimension, _config.top - 2,
                 _config.left + 12 + _maxDimension, _config.top - 5, GRAY_9);
    dsp.setFont();

    for (int i = 0; i < _maxDimension; i++) {
        if (i % (_maxDimension / _bands.perheight) == 0) {
            if (_config.align) {
#ifndef BOOMBOX_STYLE
                bandColor = (i >= (int)(_maxDimension * 0.8)) ? GRAY_9 : GRAY_1;
                dsp.fillRect(i + _config.left + 12, _config.top, h, _bands.height, bandColor);
                dsp.fillRect(i + _config.left + 12, _config.top + _bands.space, h, _bands.height, bandColor);
#endif
            } else {
                bandColor = (i < (int)(_maxDimension * 0.2)) ? GRAY_1 : GRAY_9;
                dsp.fillRect(_config.left, _config.top + _maxDimension - i - h, _bands.width, h, bandColor);
                dsp.fillRect(_config.left, _config.top + _bands.space + _maxDimension - i - h, _bands.width, h, bandColor);
            }
        }
    }

    if (_config.align) {
#ifndef BOOMBOX_STYLE
        dsp.fillRect(_config.left + 12, _config.top, _maxDimension, _bands.height, _bgcolor);
        dsp.fillRect(_config.left + 12, _config.top + _bands.space, _maxDimension, _bands.height, _bgcolor);
        for (int x = 0; x < measL_static; x++) {
            for (int y = 0; y < _bands.height; y++) {
                if ((x + y) % 2 == 0) {
                    dsp.drawPixel(_config.left + 12 + x, _config.top + y, GRAY_7);  // Jaśniejsze paski
                } else {
                    dsp.drawPixel(_config.left + 12 + x, _config.top + y, _bgcolor);
                }
            }
        }
        for (int x = 0; x < measR_static; x++) {
            for (int y = 0; y < _bands.height; y++) {
                if ((x + y) % 2 == 0) {
                    dsp.drawPixel(_config.left + 12 + x, _config.top + _bands.space + y, GRAY_7);  // Jaśniejsze paski
                } else {
                    dsp.drawPixel(_config.left + 12 + x, _config.top + _bands.space + y, _bgcolor);
                }
            }
        }
#endif
    } else {
        dsp.fillRect(_config.left, _config.top, _bands.width, _maxDimension + _bands.space + _bands.height, _bgcolor);
        for (int y_offset = 0; y_offset < measL_static; y_offset++) {
            int y = _config.top + _maxDimension - 1 - y_offset;
            for (int x = 0; x < _bands.width; x++) {
                if ((x + y_offset) % 2 == 0) {
                    dsp.drawPixel(_config.left + x, y, GRAY_7);  // Jaśniejsze paski
                } else {
                    dsp.drawPixel(_config.left + x, y, _bgcolor);
                }
            }
        }
        for (int y_offset = 0; y_offset < measR_static; y_offset++) {
            int y = _config.top + _bands.space + _maxDimension - 1 - y_offset;
            for (int x = 0; x < _bands.width; x++) {
                if ((x + y_offset) % 2 == 0) {
                    dsp.drawPixel(_config.left + x, y, GRAY_7);  // Jaśniejsze paski
                } else {
                    dsp.drawPixel(_config.left + x, y, _bgcolor);
                }
            }
        }
    }

    if (_config.align) {
        dsp.fillRect(_config.left + 12 + _peakL, _config.top, 1, _bands.height, TFT_FG);
        dsp.fillRect(_config.left + 12 + _peakR, _config.top + _bands.space, 1, _bands.height, TFT_FG);
    } else {
        dsp.fillRect(_config.left, _config.top + _maxDimension - _peakL - 1, _bands.width, 1, TFT_FG);
        dsp.fillRect(_config.left, _config.top + _bands.space + _maxDimension - _peakR - 1, _bands.width, 1, TFT_FG);
    }
}

void VuWidget::loop() {
    if (_active && !_locked) {
        _draw();
    }
}

void VuWidget::_clear() {
    uint16_t total_width = _config.align ? (_maxDimension + 12 + 4) : (_bands.width + _config.left + 2);
    uint16_t total_height = (_bands.height * 2) + _bands.space + 15 + 6;
    dsp.fillRect(_config.left, _config.top - 11, total_width, total_height + 11, _bgcolor);
}

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


/******************************************************************/
/*********************** END VU WIDGET ****************************/
/******************************************************************/
#endif

/************************
      NUM WIDGET
 ************************/
void NumWidget::init(WidgetConfig wconf, uint16_t buffsize, bool uppercase, uint16_t fgcolor, uint16_t bgcolor) {
  Widget::init(wconf, fgcolor, bgcolor);
  _buffsize = buffsize;
  _text = (char *)malloc(sizeof(char) * _buffsize);
  memset(_text, 0, _buffsize);
  _oldtext = (char *)malloc(sizeof(char) * _buffsize);
  memset(_oldtext, 0, _buffsize);
  _textwidth = _oldtextwidth = _oldleft = 0;
  _uppercase = uppercase;
  _textheight = wconf.textsize;
}

void NumWidget::setText(const char *txt) {  //rysowanie duzych cyfer VOLUME
  strlcpy(_text, txt, _buffsize);
  _getBounds();
  if (strcmp(_oldtext, _text) == 0) return;
  uint16_t realth = _textheight;  // ------------------tu wychodzi zła wysokosc czcionki
#if defined(DSP_OLED) && DSP_MODEL != DSP_SSD1322
  realth = _textheight * CHARHEIGHT;
#endif
  if (_active) dsp.fillRect(_oldleft == 0 ? _realLeft() : min(_oldleft, _realLeft()), _config.top - 20 + 1, max(_oldtextwidth, _textwidth) + 8, 20, _bgcolor);
  _oldtextwidth = _textwidth;
  _oldleft = _realLeft();
  if (_active) _draw();
}

void NumWidget::setText(int val, const char *format) {
  char buf[_buffsize];
  snprintf(buf, _buffsize, format, val);
  setText(buf);
}

void NumWidget::_getBounds() {
  _textwidth = dsp.textWidth(_text);
}

void NumWidget::_draw() {
  if (!_active) return;

  // Stałe elementy UI
  dsp.drawLine(0, 0, 70, 0, _fgcolor);
  dsp.drawLine(186, 0, 256, 0, _fgcolor);
  dsp.drawLine(88, 18, 168, 18, _fgcolor);
  dsp.drawLine(71, 1, 87, 17, _fgcolor);
  dsp.drawLine(169, 17, 185, 1, _fgcolor);

  dsp.drawBitmap(20, 4, Sp_L, 39, 48, 9);
  dsp.drawBitmap(200, 4, Sp_R, 39, 48, 9);

  dsp.setNumFont();
  dsp.setTextColor(_fgcolor, _bgcolor);
  dsp.setCursor(_realLeft(), _config.top);
  dsp.print(_text);
  strlcpy(_oldtext, _text, _buffsize);
  dsp.setFont();

  // --- Wskaźnik głośności ---
  int volume = atoi(_text);  // konwersja z tekstu
  int levels = volume / 4;   // co 4 jednostki = 1 poziom

  const int maxLevels = 25;
  const int lineWidth = 12;
  const int lineHeight = 1;
  const int levelSpacing = 1;

  const int spacingX = 105;
  const int baseX_L = 70;
  const int baseX_R = baseX_L + spacingX;
  const int baseY = 52;

  for (int i = 0; i < maxLevels; ++i) {
    int y = baseY - i * (lineHeight + levelSpacing);
    uint16_t color = (i < levels) ? _fgcolor - (i / 3) : _bgcolor;

    // Oblicz przesunięcie tylko jeśli poziom >= 18 (czyli volume >= 75)
    int shift = (i >= 18) ? (i - 17) * 2 : 0;

    // Lewa para słupków – przesuwamy w lewo
    dsp.fillRect(baseX_L - shift, y, lineWidth, lineHeight, color);
    ///dsp.fillRect(baseX_L + lineWidth - shift, y, lineWidth, lineHeight, color);

    // Prawa para słupków – przesuwamy w prawo
    dsp.fillRect(baseX_R + shift, y, lineWidth, lineHeight, color);
    //dsp.fillRect(baseX_R + lineWidth + shift, y, lineWidth, lineHeight, color);
  }
}

/**************************
      PROGRESS WIDGET
 **************************/
void ProgressWidget::_progress() {
  char buf[_width + 1];
  snprintf(buf, _width, "%*s%.*s%*s", _pg <= _barwidth ? 0 : _pg - _barwidth, "", _pg <= _barwidth ? _pg : 5, ".....", _width - _pg, "");
  _pg++;
  if (_pg >= _width + _barwidth) _pg = 0;
  setText(buf);
}

bool ProgressWidget::_checkDelay(int m, uint32_t &tstamp) {
  if (millis() - tstamp > m) {
    tstamp = millis();
    return true;
  } else {
    return false;
  }
}

void ProgressWidget::loop() {
  if (_checkDelay(_speed, _scrolldelay)) {
    _progress();
  }
}

/**************************
      CLOCK WIDGET
 **************************/
void ClockWidget::draw() {
  if (!_active) return;
  dsp.printClock(_config.top, _config.left, _config.textsize, false);
}

void ClockWidget::_draw() {
  if (!_active) return;
  dsp.printClock(_config.top, _config.left, _config.textsize, true);
}

void ClockWidget::_clear() {
  dsp.clearClock();
}
/**************************
      BITRATE WIDGET
 **************************/
void BitrateWidget::init(BitrateConfig bconf, uint16_t fgcolor, uint16_t bgcolor) {
  Widget::init(bconf.widget, fgcolor, bgcolor);
  _dimension = bconf.dimension;
  _bitrate = 0;
  _format = BF_UNCNOWN;
  dsp.charSize(bconf.widget.textsize, _charWidth, _textheight);
  memset(_buf, 0, 6);
}

void BitrateWidget::setBitrate(uint16_t bitrate) {
  _bitrate = bitrate;
  if (_bitrate > 999) _bitrate = 999;
  _draw();
}

void BitrateWidget::setFormat(BitrateFormat format) {
  _format = format;
  _draw();
}

void BitrateWidget::_draw() {
  _clear();
  if (!_active || _format == BF_UNCNOWN || _bitrate == 0) return;
  dsp.drawRect(_config.left, _config.top, _dimension, _dimension, _fgcolor);
  dsp.fillRect(_config.left, _config.top + _dimension / 2, _dimension, _dimension / 2, _fgcolor);
  dsp.setFont();
  dsp.setTextSize(_config.textsize);
  dsp.setTextColor(_fgcolor, _bgcolor);
  snprintf(_buf, 6, "%d", _bitrate);

  dsp.setCursor(_config.left + _dimension / 2 - _charWidth * strlen(_buf) / 2, _config.top + _dimension / 4 - _textheight / 2 + 1);
  dsp.print(_buf);

  dsp.setTextColor(_bgcolor, _fgcolor);
  dsp.setCursor(_config.left + _dimension / 2 - _charWidth * 3 / 2, _config.top + _dimension - _dimension / 4 - _textheight / 2);
  switch (_format) {
    case BF_MP3: dsp.print("MP3"); break;
    case BF_AAC: dsp.print("AAC"); break;
    case BF_FLAC: dsp.print("FLC"); break;
    case BF_OGG: dsp.print("OGG"); break;
    case BF_WAV: dsp.print("WAV"); break;
    case BF_OPU:  dsp.print("opu"); break;
    case BF_VOR:  dsp.print("vor"); break;
    default: break;
  }
}

void BitrateWidget::_clear() {
  dsp.fillRect(_config.left, _config.top, _dimension, _dimension, _bgcolor);
}

#endif  // #if DSP_MODEL!=DSP_DUMMY
