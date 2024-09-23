// Copyright 2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <xs1.h>
#include <platform.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <xclib.h>

#include "xua.h"
#include "xud_device.h"
#include "midiinparse.h"
#include "midioutparse.h"

on tile[MIDI_TILE] :  port p_midi_tx                        = XS1_PORT_4C;
#if(MIDI_RX_PORT_WIDTH == 4)
on tile[MIDI_TILE] :  buffered in port:4 p_midi_rx          = XS1_PORT_1F;
#elif(MIDI_RX_PORT_WIDTH == 1)
on tile[MIDI_TILE] :  buffered in port:1 p_midi_rx          = XS1_PORT_1F;
#endif
#define CLKBLK_MIDI  XS1_CLKBLK_2
on tile[MIDI_TILE] : clock    clk_midi                      = CLKBLK_MIDI;

#define MAX_TEST_COMMANDS       100
#define TEST_COMMAND_FILE_TX   "midi_tx_cmds.txt"
#define TEST_COMMAND_FILE_RX   "midi_rx_cmds.txt"

#define DEBUG       0 // Prints for debugging. Turn off for actual test

#if     DEBUG
#define dprintf(...) printf(__VA_ARGS__)
#else
#define dprintf(...)
#endif

/* See hwsupport.xc */
void board_setup();

#ifndef CABLE_NUM
#define CABLE_NUM   0
#endif

unsigned midi_in_parse_helper(unsigned midi[3], struct midi_in_parse_state &m_state){

    unsigned valid = 0;
    unsigned packed = 0;

    for(int i = 0; i < 3; i++){
        dprintf("Packing byte %d 0x%x\n", i, midi[i]);
        {valid, packed} = midi_in_parse(m_state, CABLE_NUM, midi[i]);
        if(valid){
            return packed;
        }
    }
    return 0;
}

{unsigned, unsigned} read_config_file(uint8_t commands[MAX_TEST_COMMANDS][3])
{
    unsigned tx_line_count = 0;

    FILE * movable fptr_tx = fopen(TEST_COMMAND_FILE_TX,"rt");
    if (fptr_tx == NULL) {
        dprintf("WARNING: TX command file %s not found or unable to open.\n", TEST_COMMAND_FILE_TX);
    } else {
        unsigned a,b,c;
        while (fscanf(fptr_tx, "%u %u %u\n", &a, &b, &c) == 3) {
            commands[tx_line_count][0] = a;
            commands[tx_line_count][1] = b;
            commands[tx_line_count][2] = c;
            //printf("Line %u params: 0x%x 0x%x 0x%x\n", tx_line_count, commands[tx_line_count][0], commands[tx_line_count][1], commands[tx_line_count][2]);
            tx_line_count++;
            if(tx_line_count > MAX_TEST_COMMANDS){
                printf("ERROR: Too many lines in TX command file\n");
                tx_line_count = MAX_TEST_COMMANDS;
            }
        }
    }
    fclose(move(fptr_tx));


    unsigned rx_cmd_count = 0;

    FILE * movable fptr_rx = fopen(TEST_COMMAND_FILE_RX,"rt");
    if (fptr_rx == NULL) {
        dprintf("WARNING: RX command file %s not found or unable to open.\n", TEST_COMMAND_FILE_RX);
    } else {
        if(fscanf(fptr_rx, "%u\n", &rx_cmd_count) != 1){
            printf("ERROR: Not enough or too many items in RX command file line\n");
        }
    }
    fclose(move(fptr_rx));

    return {tx_line_count, rx_cmd_count};
}

void test(chanend c_midi){
    uint8_t commands[MAX_TEST_COMMANDS][3] = {{0}};
    unsigned num_to_tx = 0;
    unsigned num_to_rx = 0;
    {num_to_tx, num_to_rx} = read_config_file(commands);
    dprintf("Sending %u MIDI command line(s) and receiving %u MIDI command(s)\n", num_to_tx, num_to_rx);

    // For MIDI Rx
    int is_ack;
    unsigned rx_packet;

    // Midi in parse state
    struct midi_in_parse_state m_state;
    reset_midi_state(m_state);

    // Counters for Rx and Tx
    unsigned tx_cmd_count = 0;
    unsigned rx_cmd_count = 0;

    timer tmr;

    int t_tx;       // Used for delay between Txs
    int tx_end;     // Used to wait for packet to have fully left
    tmr :> t_tx;
    tmr :> tx_end;

    const int max_tx_time = XS1_TIMER_HZ / 31250 * 3 * (8 + 1 + 1);  // 30 bits at 31.25 kbps is 0.96ms
    const int tx_interval = XS1_TIMER_HZ / 8000; // SoF rate on HS
    tx_end += max_tx_time; // One whole packet

    while(tx_cmd_count < num_to_tx || rx_cmd_count < num_to_rx ){
        select{
            case midi_get_ack_or_data(c_midi, is_ack, rx_packet):
                if(is_ack){
                    dprintf("ACK from Tx\n");
                    tx_cmd_count++;
                } else {
                    unsigned midi_data[3] = {0};
                    unsigned byte_count = 0;
                    // Even though this is an Rx from MIDI we use midi_out_parse so we can decode the packet
                    {midi_data[0], midi_data[1], midi_data[2], byte_count} = midi_out_parse(byterev(rx_packet));
                    dprintf("Got packet from MIDI: 0x%8x\n", rx_packet);
                    // Note this needs to always print for capfd in pytest to pick it up
                    printf("dut_midi_rx: %u %u %u\n", midi_data[0], midi_data[1], midi_data[2]);
                    rx_cmd_count++;
                    midi_send_ack(c_midi);
                }
            break;

            case tx_cmd_count < num_to_tx => tmr when timerafter(t_tx) :> int _:
                unsigned midi[] = {commands[tx_cmd_count][0], commands[tx_cmd_count][1], commands[tx_cmd_count][2]};
                // Even though this is a Tx to MIDI we use midi_in_parse_helper to form the packet from bytes
                unsigned tx_packet = midi_in_parse_helper(midi, m_state);
                midi_send_data(c_midi, byterev(tx_packet));
                dprintf("Sent packet to midi: %u %u %u (0x%8x)\n", commands[tx_cmd_count][0], commands[tx_cmd_count][1], commands[tx_cmd_count][2], tx_packet);
                t_tx += tx_interval;
                tx_end += max_tx_time;
            break;
        }
    }

    dprintf("Tx and Rx count met - exiting after last tx complete.\n");
    tmr when timerafter(tx_end) :> int _; // wait until packet definitely departed

    exit(0);
}


int main(void)
{
    chan c_midi;

    par
    {
        on tile[0]: test(c_midi);
        on tile[1]: usb_midi(p_midi_rx, p_midi_tx, clk_midi, c_midi, 0);

        // Setup HW so we can run this on the MC board
        on tile[0]: board_setup();
    }

    return 0;
}


