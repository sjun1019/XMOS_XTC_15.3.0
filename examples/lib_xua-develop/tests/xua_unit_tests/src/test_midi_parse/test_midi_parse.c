// Copyright 2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#include <stddef.h>
#include <stdio.h>

#include "xua_unit_tests.h"
#include "../../../lib_xua/src/midi/midiinparse.h"

#define NUM_CHANS       16
#define NOTE_OFF        128
#define NOTE_ON         144
#define PRESSURE        160
#define CONTROL         176
#define PROGRAM         192
#define PRESSURE_VAL    208
#define RANGE           224
#define SYSEX_SOM       240
#define SYSEX_EOM       247

#define DATA_RANGE      128
#define DATA_MASK       (DATA_RANGE - 1)

#ifndef NUM_TESTS_PER_TEST
#define NUM_TESTS_PER_TEST   30
#endif

#ifndef CABLE_NUM
#define CABLE_NUM   0
#endif

#define RANDOM_SEED     6031769

unsigned midi_in_parse_ut(unsigned midi[3]){
    // printf("Composing data: 0x%x 0x%x 0x%x\n", midi[0], midi[1], midi[2]);

    struct midi_in_parse_state m_state;
    void * mips = &m_state;
    reset_midi_state_c_wrapper(mips);

    unsigned valid = 0;
    unsigned packed = 0;

    midi_in_parse_c_wrapper(mips, CABLE_NUM, midi[0], &valid, &packed);
    // printf("Valid: %d data: %u\n", valid, packed);
    if(valid){
        return packed;
    }
    midi_in_parse_c_wrapper(mips, CABLE_NUM, midi[1], &valid, &packed);
    // printf("Valid: %d data: %u\n", valid, packed);
    if(valid){
        return packed;
    }
    midi_in_parse_c_wrapper(mips, CABLE_NUM, midi[2], &valid, &packed);
    // printf("Valid: %d data: %u\n", valid, packed);
    if(valid){
        return packed;
    }

    return 0;
}

unsigned rndm = RANDOM_SEED;


void test_midi_note(void) {
    for(int cmd = NOTE_OFF; cmd < NOTE_ON + NUM_CHANS; cmd++){
        for(int test = 0; test < NUM_TESTS_PER_TEST; test++){
            unsigned midi_ref[3] = {cmd, random(&rndm) & DATA_MASK, random(&rndm) & DATA_MASK};
            unsigned packed = midi_in_parse_ut(midi_ref);

            unsigned midi_dut[3] = {0};
            unsigned size = 0;
            midi_out_parse_c_wrapper(packed, midi_dut, &size);
            // printf("size: %d data: 0x%x 0x%x 0x%x\n", size, midi_ref[0], midi_ref[1], midi_ref[2]);
            // printf("size: %d data: 0x%x 0x%x 0x%x\n", size, midi_dut[0], midi_dut[1], midi_dut[2]);
            //TEST_ASSERT_EQUAL_UINT32_ARRAY not working!?
            for(int i = 0; i < size; i++){
                TEST_ASSERT_EQUAL_UINT32(midi_ref[i], midi_dut[i]);
            }
        }
    }
}

void test_midi_pressure(void) {
    for(int cmd = PRESSURE; cmd < PRESSURE + NUM_CHANS; cmd++){
        for(int test = 0; test < NUM_TESTS_PER_TEST; test++){
            unsigned midi_ref[3] = {cmd, random(&rndm) & DATA_MASK, random(&rndm) & DATA_MASK};
            unsigned packed = midi_in_parse_ut(midi_ref);

            unsigned midi_dut[3] = {0};
            unsigned size = 0;
            midi_out_parse_c_wrapper(packed, midi_dut, &size);
            // printf("size: %d data: 0x%x 0x%x 0x%x\n", size, midi_ref[0], midi_ref[1], midi_ref[2]);
            // printf("size: %d data: 0x%x 0x%x 0x%x\n", size, midi_dut[0], midi_dut[1], midi_dut[2]);
            //TEST_ASSERT_EQUAL_UINT32_ARRAY not working!?
            for(int i = 0; i < size; i++){
                TEST_ASSERT_EQUAL_UINT32(midi_ref[i], midi_dut[i]);
            }
        }
    }
}

void test_midi_control(void) {
    for(int cmd = CONTROL; cmd < CONTROL + NUM_CHANS; cmd++){
        for(int test = 0; test < NUM_TESTS_PER_TEST; test++){
            unsigned midi_ref[3] = {cmd, random(&rndm) & DATA_MASK, random(&rndm) & DATA_MASK};
            unsigned packed = midi_in_parse_ut(midi_ref);

            unsigned midi_dut[3] = {0};
            unsigned size = 0;
            midi_out_parse_c_wrapper(packed, midi_dut, &size);
            // printf("size: %d data: 0x%x 0x%x 0x%x\n", size, midi_ref[0], midi_ref[1], midi_ref[2]);
            // printf("size: %d data: 0x%x 0x%x 0x%x\n", size, midi_dut[0], midi_dut[1], midi_dut[2]);
            //TEST_ASSERT_EQUAL_UINT32_ARRAY not working!?
            for(int i = 0; i < size; i++){
                TEST_ASSERT_EQUAL_UINT32(midi_ref[i], midi_dut[i]);
            }
        }
    }
}

void test_midi_program(void) {
    for(int cmd = PROGRAM; cmd < PROGRAM + NUM_CHANS; cmd++){
        for(int test = 0; test < NUM_TESTS_PER_TEST; test++){
            unsigned midi_ref[3] = {cmd, random(&rndm) & DATA_MASK, random(&rndm) & DATA_MASK};
            unsigned packed = midi_in_parse_ut(midi_ref);

            unsigned midi_dut[3] = {0};
            unsigned size = 0;
            midi_out_parse_c_wrapper(packed, midi_dut, &size);
            // printf("size: %d data: 0x%x 0x%x 0x%x\n", size, midi_ref[0], midi_ref[1], midi_ref[2]);
            // printf("size: %d data: 0x%x 0x%x 0x%x\n", size, midi_dut[0], midi_dut[1], midi_dut[2]);
            //TEST_ASSERT_EQUAL_UINT32_ARRAY not working!?
            for(int i = 0; i < size; i++){
                TEST_ASSERT_EQUAL_UINT32(midi_ref[i], midi_dut[i]);
            }
        }
    }
}

void test_midi_pressure_val(void) {
    for(int cmd = PRESSURE_VAL; cmd < PRESSURE_VAL + NUM_CHANS; cmd++){
        for(int test = 0; test < NUM_TESTS_PER_TEST; test++){
            unsigned midi_ref[3] = {cmd, random(&rndm) & DATA_MASK, random(&rndm) & DATA_MASK};
            unsigned packed = midi_in_parse_ut(midi_ref);

            unsigned midi_dut[3] = {0};
            unsigned size = 0;
            midi_out_parse_c_wrapper(packed, midi_dut, &size);
            // printf("size: %d data: 0x%x 0x%x 0x%x\n", size, midi_ref[0], midi_ref[1], midi_ref[2]);
            // printf("size: %d data: 0x%x 0x%x 0x%x\n", size, midi_dut[0], midi_dut[1], midi_dut[2]);
            //TEST_ASSERT_EQUAL_UINT32_ARRAY not working!?
            for(int i = 0; i < size; i++){
                TEST_ASSERT_EQUAL_UINT32(midi_ref[i], midi_dut[i]);
            }
        }
    }
}

void test_midi_range(void) {
    for(int cmd = RANGE; cmd < RANGE + NUM_CHANS; cmd++){
        for(int test = 0; test < NUM_TESTS_PER_TEST; test++){
            unsigned midi_ref[3] = {cmd, random(&rndm) & DATA_MASK, random(&rndm) & DATA_MASK};
            unsigned packed = midi_in_parse_ut(midi_ref);

            unsigned midi_dut[3] = {0};
            unsigned size = 0;
            midi_out_parse_c_wrapper(packed, midi_dut, &size);
            // printf("size: %d data: 0x%x 0x%x 0x%x\n", size, midi_ref[0], midi_ref[1], midi_ref[2]);
            // printf("size: %d data: 0x%x 0x%x 0x%x\n", size, midi_dut[0], midi_dut[1], midi_dut[2]);
            //TEST_ASSERT_EQUAL_UINT32_ARRAY not working!?
            for(int i = 0; i < size; i++){
                TEST_ASSERT_EQUAL_UINT32(midi_ref[i], midi_dut[i]);
            }
        }
    }
}

// https://cmtext.indiana.edu/MIDI/chapter3_system_messages.php
void test_midi_sys_ex(void) {
    const unsigned MAX_SYS_EX_LENGTH = 200; // https://cycling74.com/forums/maximu-sysex-length

    for(int sys_ex_length = 1; sys_ex_length < MAX_SYS_EX_LENGTH; sys_ex_length++){
        unsigned midi_ref[1 + MAX_SYS_EX_LENGTH + 1] = {0}; // Add SOM + EOM.
        unsigned msg_len = 1 + sys_ex_length + 1;

        // Build message
        midi_ref[0] = SYSEX_SOM;
        midi_ref[1] = 0x00;                         // Extended manf ID
        midi_ref[2] = random(&rndm) & DATA_MASK;    // Manf ID 0
        midi_ref[3] = random(&rndm) & DATA_MASK;    // Manf ID 1
        for(unsigned i = 4; i < msg_len - 1; i++){
            midi_ref[i] = random(&rndm) & DATA_MASK; // Some data
        }
        midi_ref[msg_len - 1] = SYSEX_EOM;          // End of sys-ex

        unsigned midi_dut[1 + MAX_SYS_EX_LENGTH + 1 + 2] = {0}; // Add SOM + EOM. Add 2 because msg_size % 3 may be up to 2.

        struct midi_in_parse_state mips;
        reset_midi_state_c_wrapper(&mips);
        unsigned valid = 0;
        unsigned packed = 0;

        unsigned out_idx = 0;
        for(unsigned i = 0; i < msg_len; i++){
            // printf("Ref byte: %d - 0x%x\n", i, midi_ref[i]);
            midi_in_parse_c_wrapper((void * )&mips, CABLE_NUM, midi_ref[i], &valid, &packed);
            if(valid){
                unsigned size = 0;
                midi_out_parse_c_wrapper(packed, &midi_dut[out_idx], &size);
                // printf("SIZE: %u PACKED: 0x%8x, 0x%x 0x%x 0x%x\n", size, packed, midi_dut[out_idx + 0], midi_dut[out_idx + 1], midi_dut[out_idx + 2]);
                out_idx += size;
            }
        }

        for(unsigned i = 0; i < msg_len; i++){
            // printf("%d Ref: 0x%x DUT: 0x%x\n", i, midi_ref[i], midi_dut[i]);
            TEST_ASSERT_EQUAL_UINT32(midi_ref[i], midi_dut[i]);
        }
        printf("Sysex PASS length: %u\n", msg_len);
    }
}
