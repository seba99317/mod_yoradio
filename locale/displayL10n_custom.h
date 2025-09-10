#ifndef dsp_full_loc
#define dsp_full_loc
#include <pgmspace.h>
/*************************************************************************************
    HOWTO:
    Copy this file to yoRadio/locale/displayL10n_custom.h
    and modify it
*************************************************************************************/
const char mon[] PROGMEM = "pon";
const char tue[] PROGMEM = "wto";
const char wed[] PROGMEM = "śro";
const char thu[] PROGMEM = "czw";
const char fri[] PROGMEM = "pią";
const char sat[] PROGMEM = "sob";
const char sun[] PROGMEM = "nie";

const char monf[] PROGMEM = "poniedziałek";
const char tuef[] PROGMEM = "wtorek";
const char wedf[] PROGMEM = "środa";
const char thuf[] PROGMEM = "czwartek";
const char frif[] PROGMEM = "piątek";
const char satf[] PROGMEM = "sobota";
const char sunf[] PROGMEM = "niedziela";

const char jan[] PROGMEM = "sty";
const char feb[] PROGMEM = "lut";
const char mar[] PROGMEM = "mar";
const char apr[] PROGMEM = "kwi";
const char may[] PROGMEM = "maj";
const char jun[] PROGMEM = "cze";
const char jul[] PROGMEM = "lip";
const char aug[] PROGMEM = "sie";
const char sep[] PROGMEM = "wrz";
const char ocb[] PROGMEM = "paź";
const char nov[] PROGMEM = "lis";
const char dcb[] PROGMEM = "gru";

const char wn_N[]      PROGMEM = "północny";
const char wn_NE[]     PROGMEM = "północno - wschodni";
const char wn_E[]      PROGMEM = "wschodni";
const char wn_SE[]     PROGMEM = "południowo - wschodni";
const char wn_S[]      PROGMEM = "południowy";
const char wn_SW[]     PROGMEM = "południowo - zachodni";
const char wn_W[]      PROGMEM = "zachodni";
const char wn_NW[]     PROGMEM = "północno - zachodni";
const char prv[]    PROGMEM = ", podmuchy ";

const char* const dow[]     PROGMEM = { sun, mon, tue, wed, thu, fri, sat };
const char* const dowf[]    PROGMEM = { sunf, monf, tuef, wedf, thuf, frif, satf };
const char* const mnths[]   PROGMEM = { jan, feb, mar, apr, may, jun, jul, aug, sep, ocb, nov, dcb };
const char* const wind[]    PROGMEM = { wn_N, wn_NE, wn_NE, wn_E, wn_E, wn_SE, wn_SE, wn_S, wn_S, wn_SW, wn_SW, wn_W, wn_W, wn_NW, wn_NW, wn_N, wn_N }; 

const char    const_PlReady[]    PROGMEM = "[gotowy]";
const char  const_PlStopped[]    PROGMEM = "[zatrzymany]";
const char  const_PlConnect[]    PROGMEM = "[połączony]";
const char  const_DlgVolume[]    PROGMEM = "GŁOŚNOŚĆ";
const char    const_DlgLost[]    PROGMEM = "* ROZŁĄCZONY *";
const char  const_DlgUpdate[]    PROGMEM = "* AKTUALIZACJA *";
const char const_DlgNextion[]    PROGMEM = "* NEXTION *";
const char const_getWeather[]    PROGMEM = "";
const char  const_waitForSD[]    PROGMEM = "INDEKS SD";

const char        apNameTxt[]    PROGMEM = "AP NAZWA";
const char        apPassTxt[]    PROGMEM = "HASŁO";
const char       bootstrFmt[]    PROGMEM = "PRÓBUJĘ POŁĄCZYĆ Z %s";
const char        apSettFmt[]    PROGMEM = "STRONA KONFIGURACJI: HTTP://%s/";
#if EXT_WEATHER
const char       weatherFmt[]    PROGMEM = "%s, Temperatura: %+.1f\011C (Temperatura Odczuwalna: %+.0f\011C) \007 Ciśnienie: %d hPa \007 Wilgotność: %s%% \007 Wiatr  %s %.0f%s m/s  (Miasto: %s)";
#else
const char       weatherFmt[]    PROGMEM = "%s, %.1f\011C \007 ciśnienie: %d hPa \007 wilgotność: %s%%";
#endif
const char     weatherUnits[]    PROGMEM = "metric";   /* standard, metric, imperial */
const char      weatherLang[]    PROGMEM = "pl";       /* https://openweathermap.org/current#multi */

#endif
