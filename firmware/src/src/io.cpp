#include <zephyr.h>
#include "trackersettings.h"
#include "io.h"

volatile bool buttonpressed=false;
volatile int butpin;

//                  0 1  2  3  4  5  6  7  8  9  10 11 12 13
int dpintoport[] = {1,1, 1 ,1 ,1 ,1 ,1 ,0 ,0 ,0 ,1 ,1 ,1 ,0 };
int dpintopin[]  = {3,10,11,12,15,13,14,23,21,27,2 ,1 ,8 ,13};

void arduino_gpio_init(arduino_gpio_t *gpios)
{
	gpios->gpios[0] = device_get_binding("GPIO_0");
	gpios->gpios[1] = device_get_binding("GPIO_1");
}

void io_Init()
{
    pinMode(ARDUINO_INTERNAL_VDD_ENV_ENABLE, GPIO_OUTPUT);
    pinMode(ARDUINO_INTERNAL_I2C_PULLUP, GPIO_OUTPUT);
    pinMode(LED_BUILTIN, GPIO_OUTPUT);
    pinMode(ARDUINO_LEDPWR, GPIO_OUTPUT);
    pinMode(LEDR, GPIO_OUTPUT);
    pinMode(LEDG, GPIO_OUTPUT);
    pinMode(LEDB, GPIO_OUTPUT);
    digitalWrite(ARDUINO_INTERNAL_VDD_ENV_ENABLE, HIGH);
    digitalWrite(ARDUINO_INTERNAL_I2C_PULLUP, HIGH);
    digitalWrite(ARDUINO_LEDPWR,HIGH);
    digitalWrite(LEDR,HIGH);
    digitalWrite(LEDG,HIGH);
    digitalWrite(LEDB,HIGH);

    // Pins used to check timing
    pinMode(ARDUINO_A0, GPIO_OUTPUT); // PWM 0
    pinMode(ARDUINO_A1, GPIO_OUTPUT); // PWM 1
    pinMode(ARDUINO_A2, GPIO_OUTPUT); // PWM 2
    pinMode(ARDUINO_A3, GPIO_OUTPUT); // PWM 3
    pinMode(ARDUINO_A4, GPIO_INPUT); // Analog input
    pinMode(ARDUINO_A5, GPIO_INPUT); // Analog input
    pinMode(ARDUINO_A6, GPIO_INPUT); // Analog input
    pinMode(ARDUINO_A7, GPIO_INPUT); // Analog input
}

// Reset Button Pressed Flag on Read
bool wasButtonPressed()
{
    if(buttonpressed) {
        buttonpressed = false;
        return true;
    }
    return false;
}

// Reset Center

void pressButton()
{
    buttonpressed = true;
}

// Any IO Related Tasks, e.g. button
void io_Thread()
{
    int pressedtime=0;
    while(1) {
        butpin = trkset.buttonPin();

        // Make sure button pin is enabled
        if(butpin < 1 || butpin > 13 )
            return;

        // Convert from D pin to IO Pin #
        int pin = (dpintoport[butpin] * 32) + dpintopin[butpin];
        if(digitalRead(pin) == 0) {
            if(pressedtime >= BUTTON_HOLD_TIME)
                pressButton();
            else
                pressedtime += IO_PERIOD;
        } else {
            pressedtime = 0;
        }

        k_msleep(IO_PERIOD);
    }
}

