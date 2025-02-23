/*
 * This file is part of the Head Tracker distribution (https://github.com/dlktdr/headtracker)
 * Copyright (c) 2021 Cliff Blackburn
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <zephyr.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>

#include "serial.h"
#include "io.h"
#include "nano33ble.h"
#include "ble.h"

// Globals
volatile bool bleconnected=false;
volatile bool btscanonly = false;
btmodet curmode = BTDISABLE;

// UUID's
struct bt_uuid_16 ccc = BT_UUID_INIT_16(0x2902);
struct bt_uuid_16 frskyserv = BT_UUID_INIT_16(0xFFF0);
struct bt_uuid_16 frskychar = BT_UUID_INIT_16(0xFFF6);
struct bt_uuid_16 htoverridech = BT_UUID_INIT_16(0xAFF1);
struct bt_uuid_16 btbutton = BT_UUID_INIT_16(0xAFF2);
struct bt_uuid_16 jsonuuid = BT_UUID_INIT_16(0xAFF3);

// Switching modes, don't execute
volatile bool btThreadRun = false;

void bt_init()
{
    int err = bt_enable(NULL);
	if (err) {
		serialWrite("HT: Bluetooth init failed (err %d)");
        serialWrite(err);
        serialWriteln("");
		return;
	}

    serialWriteln("HT: Bluetooth initialized");
    btThreadRun = true;
}

void bt_Thread()
{
    while(1) {
        rt_sleep_us(BT_PERIOD);

        if(!btThreadRun)
            continue;

        switch(curmode) {
        case BTPARAHEAD:
            BTHeadExecute();
            break;
        case BTPARARMT:
            BTRmtExecute();
            break;
        case BTSCANONLY:
            break;
        default:
            break;
        }

        if(bleconnected)
            setLEDFlag(LED_BTCONNECTED);
        else
            clearLEDFlag(LED_BTCONNECTED);
    }
}

void BTSetMode(btmodet mode)
{
    // Requested same mode, just return
    if(mode == curmode)
        return;

    btThreadRun = false;

    // Shut Down
    switch(curmode) {
    case BTPARAHEAD:
        BTHeadStop();
        break;
    case BTPARARMT:
        BTRmtStop();
        btscanonly = false;
        break;
    case BTSCANONLY:
        BTRmtStop();
        btscanonly = false;
        break;
    default:
        break;
    }

    // Start Up
    switch(mode) {
    case BTPARAHEAD:
        BTHeadStart();
        break;
    case BTPARARMT:
        btscanonly = false;
        BTRmtStart();
        break;
    case BTSCANONLY:
        btscanonly = true;
        BTRmtStart();
    default:
        break;
    }

    btThreadRun = true;

    curmode = mode;
}

btmodet BTGetMode()
{
    return curmode;
}

bool BTGetConnected()
{
    if(curmode == BTDISABLE)
        return false;
    return bleconnected;
}

uint16_t BTGetChannel(int chno)
{
    switch(curmode) {
    case BTPARAHEAD:
        return BTHeadGetChannel(chno);
    case BTPARARMT:
        return BTRmtGetChannel(chno);
    case BTSCANONLY:
        return 0;
    default:
        break;
    }

    return 0;
}

void BTSetChannel(int channel, const uint16_t value)
{
    switch(curmode) {
    case BTPARAHEAD:
        BTHeadSetChannel(channel, value);
        break;
    case BTPARARMT:
        BTRmtSetChannel(channel, value);
        break;
    default:
        break;
    }
}

const char *BTGetAddress()
{
    switch(curmode) {
    case BTPARAHEAD:
        return BTHeadGetAddress();
    case BTPARARMT:
    case BTSCANONLY:
        return BTRmtGetAddress();
    default:
        break;
    }
    return "BT_DISABLED";
}

int8_t BTGetRSSI()
{
    switch(curmode) {
    case BTPARAHEAD:
        return BTHeadGetRSSI();
    case BTPARARMT:
        return BTRmtGetRSSI();
        break;
    default:
        break;
    }

    return -1;
}

