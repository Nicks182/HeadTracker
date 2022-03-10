#pragma once

#include <string>
#define ARDUINOJSON_USE_DOUBLE 0
#include <include/arduinojsonwrp.h>
#include "defines.h"

void serial_init();
void serial_Thread();

// ONLY use these serial write methods, they are buffered & thread safe
void serialWrite(std::string str);
void serialWrite(int val);
void serialWrite(const char *data, int len);
void serialWrite(const char *data);
void serialWrite(const char c);
void serialWriteln(const char *data = "");
void serialWriteJSON(DynamicJsonDocument &json);
void serialWriteHex(const uint8_t *data, int len);
//int serialWriteF(const char *c, ...);

void JSON_Process(char *jsonbuf);

extern struct k_mutex data_mutex;
extern DynamicJsonDocument json;

extern volatile uint16_t serial_channels[16];
