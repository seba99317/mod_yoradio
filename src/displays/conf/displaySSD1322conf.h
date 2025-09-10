/*************************************************************************************
    SSD1305 265x64 displays configuration file.
    Copy this file to yoRadio/src/displays/conf/displaySSD1322conf_custom.h
    and modify it
    More info on https://github.com/e2002/yoradio/wiki/Widgets#widgets-description
*************************************************************************************/

#ifndef displaySSD1322conf_h
#define displaySSD1322conf_h

#define DSP_WIDTH       256
#define TFT_FRAMEWDT    1
#define MAX_WIDTH       DSP_WIDTH-TFT_FRAMEWDT*2

#define HIDE_HEAPBAR
#define HIDE_VOL
//#define HIDE_WEATHER
#define HIDE_TITLE2

#if BITRATE_FULL
  #define TITLE_FIX 44
#else
  #define TITLE_FIX 0
#endif

#define bootLogoTop     68

/* SROLLS  */                            /* {{ left, top, fontsize, align }, buffsize, uppercase, width, scrolldelay, scrolldelta, scrolltime } */
const ScrollConfig metaConf       PROGMEM = {{ TFT_FRAMEWDT, TFT_FRAMEWDT, 2, WA_LEFT }, 140, false, DSP_WIDTH/2+50, 5000, 2, 25 };//pasek z tytulem stacji
const ScrollConfig title1Conf     PROGMEM = {{ 1, 18, 1, WA_LEFT }, 140, false, DSP_WIDTH/2+50, 5000, 2, 25 };        //pierwszy pasek RDS
const ScrollConfig title2Conf     PROGMEM = {{ 1, 28, 1, WA_LEFT }, 140, false, DSP_WIDTH/2+50, 5000, 2, 25 };        //drugi pasek WYKONAWCA UTWORU
const ScrollConfig playlistConf   PROGMEM = {{ TFT_FRAMEWDT, 30, 1, WA_LEFT }, 140, false, MAX_WIDTH, 500, 2, 25 };
const ScrollConfig apTitleConf    PROGMEM = {{ TFT_FRAMEWDT+1, TFT_FRAMEWDT+1, 1, WA_CENTER }, 140, false, MAX_WIDTH-2, 0, 2, 25 };
const ScrollConfig apSettConf     PROGMEM = {{ TFT_FRAMEWDT, 64-7, 1, WA_LEFT }, 140, false, MAX_WIDTH, 0, 2, 25 };
const ScrollConfig weatherConf    PROGMEM = {{ TFT_FRAMEWDT, 28, 1, WA_LEFT }, 140, false, DSP_WIDTH/2+100 , 5000, 2, 26 };  // scroling textu pogody

/* BACKGROUNGC9106DS  */                       /* {{ left, top, fontsize, align }, width, height, outlined } */
const FillConfig   metaBGConf     PROGMEM = {{ 0, 0, 0, WA_LEFT }, DSP_WIDTH/2+51, 17, false };                   //tlo po tytulu stacji
const FillConfig   volbarConf     PROGMEM = {{ 0, 0, 0, WA_LEFT }, DSP_WIDTH, 0, false };                        //konfiguracja paska VOL
const FillConfig  playlBGConf     PROGMEM = {{ 0, 66, 0, WA_LEFT }, DSP_WIDTH, 12, false };
//const FillConfig  heapbarConf     PROGMEM = {{ 0, 63, 0, WA_LEFT }, DSP_WIDTH, 1, false };

/* WIDGETS  */                           /* { left, top, fontsize, align } */
const WidgetConfig bootstrConf    PROGMEM = { 0, 64-8, 1, WA_CENTER };
//const WidgetConfig bitrateConf    PROGMEM = { 0, 3, 1, WA_RIGHT };    //wskaznik kodeke dzwieku { 30, 3, 1, WA_RIGHT};
const WidgetConfig voltxtConf     PROGMEM = { 0 ,56, 1, WA_CENTER };     //wskaznik volume
const WidgetConfig  iptxtConf     PROGMEM = { TFT_FRAMEWDT, 56, 1, WA_LEFT };
const WidgetConfig   rssiConf     PROGMEM = { 240 , 28, 1, WA_LEFT }; //56 polozenie znaczka zasiegu WiFi
const WidgetConfig numConf        PROGMEM = { 0, 40, 1, WA_CENTER };
const WidgetConfig apNameConf     PROGMEM = { 0, 18, 1, WA_CENTER };
const WidgetConfig apName2Conf    PROGMEM = { 0, 26, 1, WA_CENTER };
const WidgetConfig apPassConf     PROGMEM = { 0, 37, 1, WA_CENTER };
const WidgetConfig apPass2Conf    PROGMEM = { 0, 45, 1, WA_CENTER };
const WidgetConfig clockConf      PROGMEM = { 0, 25, 14, WA_RIGHT };  /*62 x0 y63  35 is a fixed font size. do not change 14 wys czcionki*/
const WidgetConfig vuConf         PROGMEM = { 2, 49, 1, WA_CENTER };  //polozenie wskaznika VU 

const WidgetConfig bootWdtConf    PROGMEM = { 0, 64-8*2-5, 1, WA_CENTER };
const ProgressConfig bootPrgConf  PROGMEM = { 90, 10, 4 };
//                                             {left,top,fontsize,align},wielkość widgetu
const BitrateConfig fullbitrateConf PROGMEM = {{234, 42, 1, WA_RIGHT}, 22 };

/* BANDS  */                             /* { onebandwidth, onebandheight, bandsHspace, bandsVspace, numofbands, fadespeed } */
//const VUBandsConfig bandsConf     PROGMEM = { 136, 3, 8, 1, 16, 5 };//{ 12, 48, 2, 1, 8, 3 };
const VUBandsConfig bandsConf     PROGMEM = { 210, 6, 7, 0, 1, 10 };//{ 12, 48, 2, 1, 8, 3 };

/* STRINGS  */
const char         numtxtFmt[]    PROGMEM = "%d";
const char           rssiFmt[]    PROGMEM = "WiFi %d";
const char          iptxtFmt[]    PROGMEM = "%s";//\010 znaczek ip
const char         voltxtFmt[]    PROGMEM = "\023\025%d";
const char        bitrateFmt[]    PROGMEM = "%dkBs";

/* MOVES  */                             /* { left, top, width } */
const MoveConfig    clockMove     PROGMEM = { 0, 0, -1 };
const MoveConfig   weatherMove    PROGMEM = { 0, 0, -1 };
const MoveConfig   weatherMoveVU  PROGMEM = { 0, 0, -1 };

#endif
