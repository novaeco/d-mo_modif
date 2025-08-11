#include "lgfx_efont_tw.h"
#if defined ( ARDUINO ) && !defined ( PROGMEM )
 #if __has_include(<pgmspace.h>)
  #include <pgmspace.h>
 #elif __has_include(<avr/pgmspace.h>) || defined(__AVR__)
  #include <avr/pgmspace.h>
 #else
  #include <Arduino.h>
 #endif
#endif
#ifndef PROGMEM
#define PROGMEM
#endif

#define U8G2_USE_LARGE_FONTS

/*
  Fontname: -Efont-Biwidth-Medium-R-Normal--10-100-75-75-P-50-ISO10646-1
  Copyright: (c) Copyright 2000-2003 /efont/ The Electronic Font Open Laboratory.
  Glyphs: 13557/22818
  BBX Build Mode: 0
*/
