/*
 *     SocialLedge.com - Copyright (C) 2013
 *
 *     This file is part of free software framework for embedded processors.
 *     You can use it and/or distribute it as long as this copyright header
 *     remains unmodified.  The code is free for personal use and requires
 *     permission to use in a commercial product.
 *
 *      THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *      OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *      I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *      CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *     You can reach the author of this software at :
 *          p r e e t . w i k i @ g m a i l . c o m
 */

#include "io.hpp" // All IO Class definitions
#include "bio.h"
#include "adc0.h"
#include "isr_priorities.h"



/**
 * The following diagram shows bit number corresponding to the LED
 *
 *      0
 *     ---
 *  5 | 6 | 1
 *     ---
 *  4 |   | 2
 *     ---
 *      3   *7
 */
static const unsigned char LED_DISPLAY_CHARMAP[128] =
{
        0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        0,    // 32 : Space
        0x86, // 33 : !
        0,0,0,0, 0,0,0,0, 0,0,0, // 34-44
        0x40, // 45 : -
        0x80, // 46 : .
        0x00, // 47 : /
        0x3F, // 48 : 0
        0x30, // 49 : 1
        0x5B, // 50 : 2
        0x4F, // 51 : 3
        0x66, // 52 : 4
        0x6D, // 53 : 5
        0x7C, // 54 : 6
        0x07, // 55 : 7
        0x7F, // 56 : 8
        0x67, // 57 : 9
        0,0,0,0,0,0,0, // 58-64
        0x77, // 65 : A
        0x7F, // 66 : B
        0x39, // 67 : C
        0x3F, // 68 : D
        0x79, // 69 : E
        0x71, // 70 : F
        0x3D, // 71 : G
        0x76, // 72 : H
        0x30, // 73 : I
        0x1F, // 74 : J
        0x00, // 75 : K
        0x38, // 76 : L
        0x00, // 77 : M
        0x00, // 78 : N
        0x3F, // 79 : O
        0x73, // 80 : P
        0x00, // 81 : Q
        0x00, // 82 : R
        0x6D, // 83 : S
        0x00, // 84 : T
        0x3F, // 85 : U
        0x00, // 86 : V
        0x00, // 87 : W
        0x00, // 88 : X
        0x00, // 89 : Y
        0x00, // 90 : Z

        0, 0, 0, 0, 0, 0,

        0x77, // 97 : a
        0x7F, // 98 : b
        0x39, // 99 : c
        0x3F, // 100 : d
        0x79, // 101 : e
        0x71, // 102 : f
        0x3D, // 103 : g
        0x76, // 104 : h
        0x30, // 105 : i
        0x1F, // 106 : j
        0x00, // 107 : k
        0x38, // 108 : l
        0x00, // 109 : m
        0x00, // 110 : n
        0x3F, // 111 : o
        0x73, // 112 : p
        0x00, // 113 : q
        0x00, // 114 : r
        0x6D, // 115 : s
        0x00, // 116 : t
        0x3F, // 117 : u
        0x00, // 118 : v
        0x00, // 119 : w
        0x00, // 120 : x
        0x00, // 121 : y
        0x00, // 122 : z
};



bool Acceleration_Sensor::init()
{
    const unsigned char activeModeWith100Hz = (1 << 0) | (3 << 3); // Active Mode @ 100Hz

    writeReg(Ctrl_Reg1, activeModeWith100Hz);
    const char whoAmIReg = readReg(WhoAmI);

    return (mWhoAmIExpectedValue == whoAmIReg);
}
int16_t Acceleration_Sensor::getX()
{
    return (int16_t)get16BitRegister(X_MSB) / 16;
}
int16_t Acceleration_Sensor::getY()
{
    return (int16_t)get16BitRegister(Y_MSB) / 16;
}
int16_t Acceleration_Sensor::getZ()
{
    return (int16_t)get16BitRegister(Z_MSB) / 16;
}



/**
 * The design of the IR Sensor is as follows:
 *  Timer1 captures falling edges of CAP1.0 and timestamps are saved when this happens.
 *  Every time capture happens, a 20ms match interrupt is setup/reset.  When no more
 *  signals arrive, the 20ms match interrupt will decode the timestamps and convert
 *  timestamps into binary signal.
 *
 *  Note that this doesn't decode into real data sent through the IR, but nevertheless,
 *  the decoded signal will be unique per button pressed on an IR remote.
 */
static unsigned int LAST_DECODED_IR_SIGNAL = 0;
#define TIMER1_US_PER_TICK  (100)

/**
 * Disabling this code because in case we go with TIMER1 CAPTURE for RC receiver.
 * The idea is that 6-channels can be OR'd to the capture pin, and we can use
 * the hardware to capture the timestamp of timer1 rather than capturing it
 * in software because the FreeRTOS kernel interrupt may skew the timing.
 */
#if 0
extern "C"
{
    void TIMER1_IRQHandler()
    {
        const unsigned int captureMask = (1 << 4);
        const unsigned int MR0Mask     = (1 << 0);
        const unsigned int ticksFor20Ms = (20 * 1000) / TIMER1_US_PER_TICK;

        const  unsigned short maxFallingEdgesPerIRFrame = 32;
        static unsigned short signalCount = 0;
        static unsigned int signalArray[maxFallingEdgesPerIRFrame] = {0};

        // Capture interrupt occurred:
        if(LPC_TIM1->IR & captureMask)
        {
            /**
             * Reload Match Register Interrupt to interrupt 20ms after this point of time
             * If another capture interrupt arrives later, this timer is reset again.
             */
            LPC_TIM1->MR0 = (LPC_TIM1->TC + ticksFor20Ms);

            // Just store the timestamp of this signal
            if(signalCount < maxFallingEdgesPerIRFrame) {
                signalArray[signalCount++] = LPC_TIM1->CR0;
            }

            // Clear the Timer Capture interrupt
            LPC_TIM1->IR = captureMask;
        }
        // Timeout Interrupt to decode the signal
        else if(LPC_TIM1->IR & MR0Mask)
        {
            if(signalCount > 1)
            {
                /**
                 * Time to decode the signals at this timeout
                 * Calculate differences of falling edges
                 */
                for(int i = 0; i < signalCount-1; i++) {
                    signalArray[i] = signalArray[i+1] - signalArray[i];
                }

                /**
                 * First falling edge value should indicate binary 0.
                 * So anything higher than 50% of this value is considered binary 1.
                 */
                const unsigned int binary1Threshold = signalArray[1] + (signalArray[1]/2);
                unsigned int decodedSignal = 0;
                for(unsigned short i=0; i < signalCount-1; i++) {
                    if(signalArray[i] > binary1Threshold) {
                        //printf("%i, ", signalArray[i]);
                        decodedSignal |= (1 << i);
                    }
                }
                LAST_DECODED_IR_SIGNAL = decodedSignal;
            }

            // Clear the Match Interrupt and signal count
            signalCount = 0;
            LPC_TIM1->IR = MR0Mask;
        }
        else
        {
            // Log error of unexpected interrupt
        }
    }
}


/**
 * IR Sensor is attached to P1.18 - CAP1.0, so it needs TIMER1 to capture the times on P1.18
 */
bool IR_Sensor::init()
{
    // Enable Timer1 that is used to capture CAP1.0 signal
    lpc_pconp(pconp_timer1, true);
    lpc_pclk(pclk_timer1, clkdiv_1);

    LPC_TIM1->TCR = 1;              // Enable Timer
    LPC_TIM1->CTCR = 0;             // Increment on PCLK
    LPC_TIM1->PR = sys_get_cpu_clock() / (1000*1000/TIMER1_US_PER_TICK);

    LPC_TIM1->CCR &= ~(7 << 0);            // Clear Bits 2:1:0
    LPC_TIM1->CCR |=  (1 << 2) | (1 << 1); // Enable Falling Edge capture0 with interrupt

    // Enable MR0 interrupt
    LPC_TIM1->MR0 = 0;
    LPC_TIM1->MCR |= (1 << 0);

    // Select P1.18 as CAP1.0 by setting bits 5:4 to 0b11
    LPC_PINCON->PINSEL3 |= (3 << 4);

    // Finally, enable interrupt of Timer1 to interrupt upon falling edge capture
    NVIC_EnableIRQ(TIMER1_IRQn);

    return true;
}
#else
bool IR_Sensor::init()
{
    return false;
}
#endif

bool IR_Sensor::isIRCodeReceived()
{
    return (0 != LAST_DECODED_IR_SIGNAL);
}
unsigned int IR_Sensor::getLastIRCode()
{
    unsigned int signal = LAST_DECODED_IR_SIGNAL;
    LAST_DECODED_IR_SIGNAL = 0;
    return signal;
}



bool LED_Display::init()
{
    bool devicePresent = checkDeviceResponse();
    if(devicePresent)
    {
        const unsigned char cfgAsOutput = 0x00;
        writeReg(cfgPort0, cfgAsOutput);
        writeReg(cfgPort1, cfgAsOutput);

        setLeftDigit('.');
        setRightDigit('.');
    }

    return devicePresent;
}
void LED_Display::clear()
{
    setLeftDigit(' ');
    setRightDigit(' ');
}
void LED_Display::setNumber(char num)
{
    num %= 100;
    setRightDigit(num%10 + '0');

    if(num < 10)
        setLeftDigit(' ');
    else
        setLeftDigit (num/10 + '0');
}
void LED_Display::setLeftDigit(char alpha)
{
    mNumAtDisplay[0] = alpha;
    writeReg(outputPort1, LED_DISPLAY_CHARMAP[(unsigned) (alpha & 0x7F) ]);
}
void LED_Display::setRightDigit(char alpha)
{
    mNumAtDisplay[1] = alpha;
    writeReg(outputPort0, LED_DISPLAY_CHARMAP[(unsigned) (alpha & 0x7F) ]);
}




bool LED::init()
{
    /* Pins initialized by bio.h */
    return true;
}
void LED::on(int ledNum)
{
    mLedValue |= (1 << (ledNum-1));
    setAll(mLedValue);
}
void LED::off(int ledNum)
{
    mLedValue &= ~(1 << (ledNum-1));
    setAll(mLedValue);
}
void LED::set(int ledNum, bool o)
{
    if (o) on(ledNum);
    else   off(ledNum);
}
void LED::setAll(char value)
{
    mLedValue = value & 0x0F;
    LPC_GPIO1->FIOSET = BIO_LED_PORT1_MASK;

    /* LEDs are active low */
    if(mLedValue & (1 << 0)) LPC_GPIO1->FIOCLR = (1 << 0);
    if(mLedValue & (1 << 1)) LPC_GPIO1->FIOCLR = (1 << 1);
    if(mLedValue & (1 << 2)) LPC_GPIO1->FIOCLR = (1 << 4);
    if(mLedValue & (1 << 3)) LPC_GPIO1->FIOCLR = (1 << 8);
}
uint8_t LED::getValues(void) const
{
    return mLedValue;
}


bool Light_Sensor::init()
{
    return true;
}
uint16_t Light_Sensor::getRawValue()
{
    return adc0_get_reading(BIO_LIGHT_ADC_CH_NUM);
}
uint8_t Light_Sensor::getPercentValue()
{
    const unsigned int maxAdcValue = 4096;
    return (getRawValue() * 100) / maxAdcValue;
}



bool Switches::init()
{
    /* Pins initialized by bio.h */
    return true;
}
uint8_t Switches::getSwitchValues()
{
    /* Return contiguous bits 0-3 from scattered switch pins */
    const int swValues = LPC_GPIO1->FIOPIN;
    const int p9_p10  = (swValues >> 9)  & 0x3;
    const int p14_p15 = (swValues >> 14) & 0x3;
    return (p9_p10 | (p14_p15 << 2));
}
bool Switches::getSwitch(int num)
{
    if (num >= 1 && num <= 4) {
        return getSwitchValues() & (1 << (num-1));
    }
    else {
        return false;
    }
}



bool I2C_Temp::init()
{
    const unsigned char temperatureCfgRegPtr = 0x01;
    const unsigned char oneShotShutdownMode = 0x81;
    const unsigned char expectedBitsThatAreNotZero = 0x60; // R1:R0 bits are 1s at startup

    // Enter one-shot shutdown mode.
    writeReg(temperatureCfgRegPtr, oneShotShutdownMode);

    const unsigned char cfgRegByte0 = readReg(temperatureCfgRegPtr);
    return (0 != (cfgRegByte0 & expectedBitsThatAreNotZero));
}
float I2C_Temp::getCelsius()
{
    // Get signed 16-bit data of temperature register pointer
    const unsigned char temperatureRegsiterPtr = 0x00;
    signed short temperature = get16BitRegister(temperatureRegsiterPtr);

    // Trigger next conversion:
    const unsigned char temperatureCfgRegPtr = 0x01;
    const unsigned char oneShotShutdownMode = 0x81;
    writeReg(temperatureCfgRegPtr, oneShotShutdownMode);

    // Temperature data is in bits 15:3 which contains signed 16-bit data
    temperature /= 16;

    // Each bit is of 0.0625 degree per bit resolution
    return (0.0625F * temperature) + mOffsetCelcius;
}

float I2C_Temp::getFarenheit()
{
    return (getCelsius() * 9.0F / 5) + 32;
}

