#pragma once

/*  Infrarotempfänger für Nano Motorsteuerung
 *   
 *  This code is based on MinimalReceiver (Armin Joachimsmeyer)
 *  Modified by 'Der Hobbyelektroniker' for 'Nano Motorsteuerung'
 *
 *  Small memory footprint and no timer usage!
 *  
 *  Receives IR protocol data of NEC protocol using pin change interrupts.
 *  On complete received IR command the function handleReceivedIRData(uint16_t aAddress, uint8_t aCommand, bool isRepetition)
 *  is called in Interrupt context but with interrupts being enabled to enable use of delay() etc.
 *  !!!!!!!!!!!!!!!!!!!!!!
 *  Functions called in interrupt context should be running as short as possible,
 *  so if you require longer action, save the data (address + command) and handle it in the main loop.
 *  !!!!!!!!!!!!!!!!!!!!!
 *
 *
 *  Copyright (C) 2020-2021  Armin Joachimsmeyer
 *  armin.joachimsmeyer@gmail.com
 *
 *  This file is part of IRMP https://github.com/ukw100/IRMP.
 *  This file is part of Arduino-IRremote https://github.com/Arduino-IRremote/Arduino-IRremote.
 *
 *  IRMP is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/gpl.html>.
 *
 */

#include <Arduino.h>
#include "digitalWriteFast.h"

/*
 * This function is called if a complete command was received and must be implemented by the including file (user code)
 */
void handleReceivedTinyIRData(uint16_t aAddress, uint8_t aCommand, bool isRepeat);

uint8_t ir_input_pin;

union WordUnion {
    struct {
        uint8_t LowByte;
        uint8_t HighByte;
    } UByte;
    struct {
        int8_t LowByte;
        int8_t HighByte;
    } Byte;
    uint8_t UBytes[2];
    int8_t Bytes[2];
    uint16_t UWord;
    int16_t Word;
    uint8_t * BytePointer;
};

/**
 * Union to specify parts / manifestations of a 32 bit Long without casts and shifts.
 * It also supports the compiler generating small code.
 */
union LongUnion {
    struct {
        uint8_t LowByte;
        uint8_t MidLowByte;
        uint8_t MidHighByte;
        uint8_t HighByte;
    } UByte;
    struct {
        int8_t LowByte;
        int8_t MidLowByte;
        int8_t MidHighByte;
        int8_t HighByte;
    } Byte;
    struct {
        uint8_t LowByte;
        WordUnion MidWord;
        uint8_t HighByte;
    } ByteWord;
    struct {
        int16_t LowWord;
        int16_t HighWord;
    } Word;
    struct {
        WordUnion LowWord;
        WordUnion HighWord;
    } WordUnion;
    struct {
        uint16_t LowWord;
        uint16_t HighWord;
    } UWord;
    uint8_t UBytes[4];
    int8_t Bytes[4];
    uint16_t UWords[2];
    int16_t Words[2];
    uint32_t ULong;
    int32_t Long;
};

// LSB first, 1 start bit + 16 bit address + 8 bit data + 8 bit inverted data + 1 stop bit.
// see: https://www.sbprojects.net/knowledge/ir/nec.php

#define NEC_ADDRESS_BITS        16 // 16 bit address or 8 bit address and 8 bit inverted address
#define NEC_COMMAND_BITS        16 // Command and inverted command

#define NEC_BITS                (NEC_ADDRESS_BITS + NEC_COMMAND_BITS)
#define NEC_UNIT                560

#define NEC_HEADER_MARK         (16 * NEC_UNIT) // 9000
#define NEC_HEADER_SPACE        (8 * NEC_UNIT)  // 4500

#define NEC_BIT_MARK            NEC_UNIT
#define NEC_ONE_SPACE           (3 * NEC_UNIT)  // 1690
#define NEC_ZERO_SPACE          NEC_UNIT

#define NEC_REPEAT_HEADER_SPACE (4 * NEC_UNIT)  // 2250
#define NEC_REPEAT_PERIOD       110000 // Not used yet - Commands are repeated every 110 ms (measured from start to start) for as long as the key on the remote control is held down.

/*
 * Macros for comparing timing values
 */
#define lowerValue25Percent(aDuration)   (aDuration - (aDuration / 4))
#define upperValue25Percent(aDuration)   (aDuration + (aDuration / 4))
#define lowerValue(aDuration)   (aDuration - (aDuration / 2))
#define upperValue(aDuration)   (aDuration + (aDuration / 2))

/*
 * The states for the state machine
 */
#define IR_RECEIVER_STATE_WAITING_FOR_START_MARK        0
#define IR_RECEIVER_STATE_WAITING_FOR_START_SPACE       1
#define IR_RECEIVER_STATE_WAITING_FOR_FIRST_DATA_MARK   2
#define IR_RECEIVER_STATE_WAITING_FOR_DATA_SPACE        3
#define IR_RECEIVER_STATE_WAITING_FOR_DATA_MARK         4
#define IR_RECEIVER_STATE_WAITING_FOR_STOP_MARK         5
/**
 * Control and data variables of the state machine for TinyReceiver
 */
struct TinyIRReceiverStruct {
    /*
     * State machine
     */
    uint32_t LastChangeMicros;      ///< microseconds of last Pin Change Interrupt.
    uint8_t IRReceiverState;        ///< the state of the state machine.
    uint8_t IRRawDataBitCounter;
    /*
     * Data
     */
    uint32_t IRRawDataMask;
    LongUnion IRRawData;
    bool IRRepeatDetected;
};

TinyIRReceiverStruct TinyIRReceiverControl;

/**
 * Declaration of the callback function provided by the user application.
 * It is called every time a complete IR command or repeat was received.
 */
void handleReceivedIRData(uint16_t aAddress, uint8_t aCommand, bool isRepetition);

/**
 * The ISR of TinyIRRreceiver.
 * It handles the NEC protocol decoding and calls the user callback function on complete.
 * 5 us + 3 us for push + pop for a 16MHz ATmega
 */
void IRPinChangeInterruptHandler(void) {
    /*
     * Save IR input level
     * Negative logic, true / HIGH means inactive / IR space, LOW / false means IR mark.
     */
    uint_fast8_t tIRLevel = digitalReadFast(ir_input_pin);

    /*
     * 1. compute microseconds after last change
     */
    uint32_t tCurrentMicros = micros();
    uint16_t tMicrosOfMarkOrSpace = tCurrentMicros - TinyIRReceiverControl.LastChangeMicros;
    TinyIRReceiverControl.LastChangeMicros = tCurrentMicros;

    uint8_t tState = TinyIRReceiverControl.IRReceiverState;

    if (tIRLevel == LOW) {
        /*
         * We have a mark here
         */
        if (tMicrosOfMarkOrSpace > 2 * NEC_HEADER_MARK) {
            // timeout -> must reset state machine
            tState = IR_RECEIVER_STATE_WAITING_FOR_START_MARK;
        }
        if (tState == IR_RECEIVER_STATE_WAITING_FOR_START_MARK) {
            // We are at the beginning of the header mark, check timing at the next transition
            tState = IR_RECEIVER_STATE_WAITING_FOR_START_SPACE;
        }

        else if (tState == IR_RECEIVER_STATE_WAITING_FOR_FIRST_DATA_MARK) {
            if (tMicrosOfMarkOrSpace >= lowerValue25Percent(NEC_HEADER_SPACE)
                    && tMicrosOfMarkOrSpace <= upperValue25Percent(NEC_HEADER_SPACE)) {
                /*
                 * We have a valid data header space here -> initialize data
                 */
                TinyIRReceiverControl.IRRawDataBitCounter = 0;
                TinyIRReceiverControl.IRRawData.ULong = 0;
                TinyIRReceiverControl.IRRawDataMask = 1;
                TinyIRReceiverControl.IRRepeatDetected = false;
                tState = IR_RECEIVER_STATE_WAITING_FOR_DATA_SPACE;
            } else if (tMicrosOfMarkOrSpace >= lowerValue25Percent(NEC_REPEAT_HEADER_SPACE)
                    && tMicrosOfMarkOrSpace <= upperValue25Percent(NEC_REPEAT_HEADER_SPACE)
                    && TinyIRReceiverControl.IRRawDataBitCounter >= NEC_BITS) {
                /*
                 * We have a repeat header here and no broken receive before -> set repeat flag
                 */
                TinyIRReceiverControl.IRRepeatDetected = true;
                tState = IR_RECEIVER_STATE_WAITING_FOR_DATA_SPACE;
            } else {
                // This parts are optimized by the compiler into jumps to one code :-)
                // Wrong length -> reset state
                tState = IR_RECEIVER_STATE_WAITING_FOR_START_MARK;
            }
        }

        else if (tState == IR_RECEIVER_STATE_WAITING_FOR_DATA_MARK) {
            // Check data space length
            if (tMicrosOfMarkOrSpace >= lowerValue(NEC_ZERO_SPACE) && tMicrosOfMarkOrSpace <= upperValue(NEC_ONE_SPACE)) {
                // We have a valid bit here
                tState = IR_RECEIVER_STATE_WAITING_FOR_DATA_SPACE;
                if (tMicrosOfMarkOrSpace >= 2 * NEC_UNIT) {
                    // we received a 1
                    TinyIRReceiverControl.IRRawData.ULong |= TinyIRReceiverControl.IRRawDataMask;
                } else {
                    // we received a 0 - empty code for documentation
                }
                // prepare for next bit
                TinyIRReceiverControl.IRRawDataMask = TinyIRReceiverControl.IRRawDataMask << 1;
                TinyIRReceiverControl.IRRawDataBitCounter++;
            } else {
                // Wrong length -> reset state
                tState = IR_RECEIVER_STATE_WAITING_FOR_START_MARK;
            }
        } else {
            // error wrong state for the received level, e.g. if we missed one change interrupt -> reset state
            tState = IR_RECEIVER_STATE_WAITING_FOR_START_MARK;
        }
    }

    else {
        /*
         * We have a space here
         */
        if (tState == IR_RECEIVER_STATE_WAITING_FOR_START_SPACE) {
            /*
             * Check length of header mark here
             */
            if (tMicrosOfMarkOrSpace >= lowerValue25Percent(NEC_HEADER_MARK)
                    && tMicrosOfMarkOrSpace <= upperValue25Percent(NEC_HEADER_MARK)) {
                tState = IR_RECEIVER_STATE_WAITING_FOR_FIRST_DATA_MARK;
            } else {
                // Wrong length of header mark -> reset state
                tState = IR_RECEIVER_STATE_WAITING_FOR_START_MARK;
            }
        }

        else if (tState == IR_RECEIVER_STATE_WAITING_FOR_DATA_SPACE) {
            // Check data mark length
            if (tMicrosOfMarkOrSpace >= lowerValue(NEC_BIT_MARK) && tMicrosOfMarkOrSpace <= upperValue(NEC_BIT_MARK)) {
                /*
                 * We have a valid mark here, check for transmission complete
                 */
                if (TinyIRReceiverControl.IRRawDataBitCounter >= NEC_BITS || TinyIRReceiverControl.IRRepeatDetected) {
                    /*
                     * Code complete -> call callback, no parity check!
                     */
                    // Reset state for new start
                    tState = IR_RECEIVER_STATE_WAITING_FOR_START_MARK;
                    
#if !defined(ARDUINO_ARCH_MBED)
                    interrupts();
#endif
                    /*
                     * Address reduction to 8 bit
                     */
                    if (TinyIRReceiverControl.IRRawData.UByte.LowByte
                            == (uint8_t) (~TinyIRReceiverControl.IRRawData.UByte.MidLowByte)) {
                        // standard 8 bit address NEC protocol
                        TinyIRReceiverControl.IRRawData.UByte.MidLowByte = 0; // Address is the first 8 bit
                    }

                    /*
                     * Call user provided callback here
                     */
                    handleReceivedTinyIRData(TinyIRReceiverControl.IRRawData.UWord.LowWord,
                            TinyIRReceiverControl.IRRawData.UByte.MidHighByte, TinyIRReceiverControl.IRRepeatDetected);

                } else {
                    // not finished yet
                    tState = IR_RECEIVER_STATE_WAITING_FOR_DATA_MARK;
                }
            } else {
                // Wrong length -> reset state
                tState = IR_RECEIVER_STATE_WAITING_FOR_START_MARK;
            }
        } else {
            // error wrong state for the received level, e.g. if we missed one change interrupt -> reset state
            tState = IR_RECEIVER_STATE_WAITING_FOR_START_MARK;
        }
    }

    TinyIRReceiverControl.IRReceiverState = tState;
}

/**
 * Initializes hardware interrupt generation according to ir_input_pin or use attachInterrupt() function.
 */
void initPCIInterruptForTinyReceiver(uint8_t pin) {
  ir_input_pin = pin;
  pinModeFast(ir_input_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ir_input_pin), IRPinChangeInterruptHandler, CHANGE);
  IRPinChangeInterruptHandler();
}
