#pragma once

#include "defines.h"
#include "bluetooth/btparahead.h"
#include "bluetooth/btpararmt.h"

#define LEN_BLUETOOTH_ADDR              16
#define MAX_BLUETOOTH_DISTANT_ADDR      6
#define BLUETOOTH_LINE_LENGTH           32
#define BT_CHANNELS 8

extern volatile bool bleconnected;
extern volatile bool btscanonly;

typedef enum {
    BTDISABLE=0,
    BTPARAHEAD,
    BTPARARMT,
    BTSCANONLY,
    BTCOUNT
} btmodet;

void bt_Thread();
void bt_Init();
void BTSetMode(btmodet mode);
btmodet BTGetMode();
uint16_t BTGetChannel(int chno);
void BTSetChannel(int channel, const uint16_t value);
bool BTGetConnected();
const char *BTGetAddress();
int8_t BTGetRSSI();