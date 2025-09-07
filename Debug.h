#pragma once

#define DEBUG_MODE true  // Altere para false para desativar os prints

#if DEBUG_MODE
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
  #define DEBUG_PRINT2(x, y) Serial.print(x, y)
  #define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINT2(x, y)
  #define DEBUG_PRINTF(...)
#endif
