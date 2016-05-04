#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#include "../include/phil_boardSH.h"


const unsigned int fLength_header      = 1;//sizeof(foo.header);// length of field "header" in bytes
const unsigned int fLength_payloadSize = 1;//sizeof(foo.payloadSize);
const unsigned int fLength_checksum    = 1;


static PckDescr tcpCommands[TCP_COMMANDS_COUNT];
static PckDescr tcpReplies[TCP_REPLIES_COUNT];
static PckDescr udpCommands[UDP_COMMANDS_COUNT];
static PckDescr udpReplies[UDP_REPLIES_COUNT];
static PckDescr udpBroadcasts[UDP_BROADCASTS_COUNT];



static unsigned int getStaticFieldsSize(int payLoadSize) {
	if (payLoadSize > 0)
		return fLength_header + fLength_payloadSize + fLength_checksum;
	else
		return fLength_header + fLength_payloadSize;
}

static void setChecksum(CharBuff* p) {
	int checksum = 0;
	int i;
	for (i=0 ; i<(p->size-1); i++) {
		checksum -= p->content[i];
	}
	p->content[p->size-1] = checksum;
}
	
void init(unsigned int numOfBoards) {
			
	PckDescr* descr;
	
	/* TCP COMMANDS :  */
	
	descr = &tcpCommands[GET_BOARD_TYPE];
	descr->header = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x10;
    descr->command[1]  = 0xf1;
    descr->commandSize = 2;
    descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;
    
    descr = &tcpCommands[GET_FIRMWARE_VERSION];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x11;
    descr->command[1]  = 0xf0;
    descr->commandSize = 2;
    descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

    descr = &tcpCommands[SET_FIRMWARE_VERSION];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x51;
    descr->commandSize = 1;
	descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[CLEAR_BOARD_FAULT];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x52;
	descr->command[1]  = 0xaf;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_BOARD_FAULT];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x12;
	descr->command[1]  = 0xef;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[SET_PID_GAINS];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x53;
    descr->commandSize = 1;
	descr->payloadSize = 0x0d;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_PID_GAINS];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x13;
    descr->commandSize = 1;
	descr->payloadSize = 1;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[SET_PID_GAIN_SCALE];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x54;
    descr->commandSize = 1;
	descr->payloadSize = 0x0d;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_PID_GAIN_SCALE];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x14;
    descr->commandSize = 1;
	descr->payloadSize = 1;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

    descr = &tcpCommands[SET_ILIM_GAIN];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x55;
    descr->commandSize = 1;
	descr->payloadSize = 5;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

    descr = &tcpCommands[GET_ILIM_GAIN];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x15;
    descr->commandSize = 1;
	descr->payloadSize = 1;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_PID_ERROR];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x16;
	descr->command[1]  = 0xeb;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_PID_OUTPUT];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x17;
	descr->command[1]  = 0xea;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_ENCODER_POSITION];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x18;
	descr->command[1]  = 0xe9;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_ENCODER_VELOCITY];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x19;
	descr->command[1]  = 0xe8;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_MEASURED_TORQUE];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x1a;
	descr->command[1]  = 0xe7;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_DESIRED_POSITION];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x1b;
	descr->command[1]  = 0xe6;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_DESIRED_VELOCITY];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x1c;
	descr->command[1]  = 0xe5;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_DESIRED_TORQUE];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x1d;
	descr->command[1]  = 0xe4;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[SET_ACCEL];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x5e;
    descr->commandSize = 1;
	descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_ACCEL];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x1e;
	descr->command[1]  = 0xe3;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[SET_MIN_POSITION];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x60;
    descr->commandSize = 1;
	descr->payloadSize = 4;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_MIN_POSITION];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x20;
	descr->command[1]  = 0xe1;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[SET_MAX_POSITION];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x61;
    descr->commandSize = 1;
	descr->payloadSize = 4;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_MAX_POSITION];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x21;
	descr->command[1]  = 0xe0;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[SET_MAX_VELOCITY];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x62;
    descr->commandSize = 1;
	descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_MAX_VELOCITY];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x22;
	descr->command[1]  = 0xdf;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[SET_MAX_TORQUE];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x63;
    descr->commandSize = 1;
	descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_MAX_TORQUE];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x23;
	descr->command[1]  = 0xde;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[SET_MIN_VELOCITY];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x6d;
	descr->commandSize = 1;
	descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_MIN_VELOCITY];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x2f;
	descr->command[1]  = 0xd2;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_PID_OFFSET];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x24;
	descr->command[1]  = 0xdd;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[SET_CURRENT_LIMIT];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x65;
    descr->commandSize = 1;
	descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_CURRENT_LIMIT];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x25;
	descr->command[1]  = 0xdc;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[SET_BCAST_RATE];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x66;
    descr->commandSize = 1;
	descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_BCAST_RATE];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x26;
	descr->command[1]  = 0xdb;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[SET_BCAST_POLICY];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x67;
    descr->commandSize = 1;
	descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_BCAST_POLICY];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x27;
	descr->command[1]  = 0xda;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[SET_ENCODER_LINES];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x68;
	descr->commandSize = 1;
	descr->payloadSize = 4;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_ENCODER_LINES];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x28;
	descr->command[1]  = 0xd9;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[SET_MOTOR_POLES];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x69;
	descr->commandSize = 1;
	descr->payloadSize = 1;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_MOTOR_POLES];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x29;
	descr->command[1]  = 0xd8;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[SET_ANALOG_INPUTS];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x6a;
    descr->commandSize = 1;
	descr->payloadSize = 1;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[DO_RECALCULATE_POSITION];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x50;
    descr->commandSize = 1;
	descr->payloadSize = 1;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_ANALOG_INPUTS];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x2a;
	descr->command[1]  = 0xd7;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[CMD_UPGRADE];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x01;
	descr->command[1]  = 0xfc;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[SAVE_PARAMS_TO_FLASH];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x2b;
	descr->command[1]  = 0xd6;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[LOAD_PARAMS_FROM_FLASH];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x2c;
	descr->command[1]  = 0xd5;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[LOAD_DEFAULT_PARAMS];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x2d;
	descr->command[1]  = 0xd4;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

    descr = &tcpCommands[CONTROLLER_RUN];
	descr->header = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x32;
    descr->command[1]  = 0xcf;
    descr->commandSize = 2;
    descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;
    
    descr = &tcpCommands[CONTROLLER_IDLE];
	descr->header = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x33;
    descr->command[1]  = 0xce;
    descr->commandSize = 2;
    descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[ENABLE_PWM_PAD];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x34;
	descr->command[1]  = 0xcd;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[DISABLE_PWM_PAD];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x35;
	descr->command[1]  = 0xcc;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[DO_CALIBRATE];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x6b;
    descr->commandSize = 1;
	descr->payloadSize = 5;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[SET_CALIBRATION_CURRENT];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x6c;
    descr->commandSize = 1;
	descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_CALIBRATION_CURRENT];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x2e;
	descr->command[1]  = 0xd3;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[SET_ABSOLUTE_ZERO];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x84;
    descr->commandSize = 1;
	descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_ABSOLUTE_ZERO];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x44;
	descr->command[1]  = 0xbd;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[SET_TORQUE_ACCEL];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x85;
    descr->commandSize = 1;
	descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_TORQUE_ACCEL];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x45;
	descr->command[1]  = 0xbc;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[SET_MOTOR_TYPE];
	descr->header = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x86;
    descr->commandSize = 1;
	descr->payloadSize = 1;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_MOTOR_TYPE];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x46;
	descr->command[1]  = 0xbb;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[SET_TORQUE_ONOFF];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x76;
    descr->commandSize = 1;
	descr->payloadSize = 1;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_TORQUE_ONOFF];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x7E;
	descr->command[1]  = 0x83;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[SET_MOTOR_CONFIG];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x77;
    descr->commandSize = 1;
	descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_MOTOR_CONFIG];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x7F;
	descr->command[1]  = 0x82;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[SET_PD_LINK_GAINS];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x72;
    descr->commandSize = 1;
	descr->payloadSize = 8;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_PD_LINK_GAINS];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x7A;
	descr->command[1]  = 0x87;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[SET_MIN_ABS_POSITION];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x8C;
    descr->commandSize = 1;
	descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_MIN_ABS_POSITION];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x4C;
	descr->command[1]  = 0xB5;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[SET_MAX_ABS_POSITION];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x8D;
    descr->commandSize = 1;
	descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_MAX_ABS_POSITION];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x4D;
	descr->command[1]  = 0xB4;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	////////////////////////  FT Sensor messages /////////////////////////////////

	descr = &tcpCommands[CALIBRATE_OFFSETS];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x3b;
	descr->command[1]  = 0xc6;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[SET_BOARD_NUMBER];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x88;
    descr->commandSize = 1;
	descr->payloadSize = 1;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_CALIBRATION_OFFSETS];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x4e;
	descr->command[1]  = 0xb3;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[SET_RESOLUTION];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x8f;
    descr->commandSize = 1;
	descr->payloadSize = 1;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_RESOLUTION];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x4f;
	descr->command[1]  = 0xb2;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[SET_TEMP_FACTORS];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x82;
    descr->commandSize = 1;
	descr->payloadSize = 0X0c;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_TEMP_FACTORS];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x42;
	descr->command[1]  = 0xbf;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_CAL_TEMP];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x48;
	descr->command[1]  = 0xb9;
    descr->commandSize = 2;
	descr->payloadSize = 0;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[SET_CONVERSION_FACTORS];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x83;
    descr->commandSize = 1;
	descr->payloadSize = 0X07;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_CONVERSION_FACTORS];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x43;
    descr->commandSize = 1;
	descr->payloadSize = 1;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[SET_AVARAGE_SAMPLES];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x30;
    descr->commandSize = 1;
	descr->payloadSize = 1;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_AVARAGE_SAMPLES];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x3C;
	descr->command[1]  = 0xC5;
    descr->commandSize = 2;
	descr->payloadSize = 1;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[SET_MATRIX_ROW];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x31;
    descr->commandSize = 1;
	descr->payloadSize = 0x19;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpCommands[GET_MATRIX_ROW];
	descr->header  = HEADER_TCP_COMMAND;
	descr->command[0]  = 0x38;
    descr->commandSize = 1;
	descr->payloadSize = 1;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;


	////////////////////////  FT Sensor messages /////////////////////////////////


    /* TCP REPLIES :  */
    descr = &tcpReplies[REPLY_BOARD_TYPE];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0x90;
    descr->commandSize = 1;
    descr->payloadSize = 1;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;
    
    descr = &tcpReplies[REPLY_FIRMWARE_VERSION];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0x91;
    descr->commandSize = 1;
    descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpReplies[REPLY_BOARD_FAULT];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0x92;
    descr->commandSize = 1;
    descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpReplies[REPLY_PID_GAINS];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0x93;
    descr->commandSize = 1;
    descr->payloadSize = 0x0d;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpReplies[REPLY_PID_GAIN_SCALE];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0x94;
    descr->commandSize = 1;
    descr->payloadSize = 0x0d;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

    descr = &tcpReplies[REPLY_ILIM_GAIN];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0x95;
    descr->commandSize = 1;
    descr->payloadSize = 5;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

    descr = &tcpReplies[REPLY_PID_ERROR];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0x96;
    descr->commandSize = 1;
    descr->payloadSize = 4;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

    descr = &tcpReplies[REPLY_PID_OUTPUT];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0x97;
    descr->commandSize = 1;
    descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpReplies[REPLY_ENCODER_POSITION];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0x98;
    descr->commandSize = 1;
    descr->payloadSize = 4;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpReplies[REPLY_ENCODER_VELOCITY];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0x99;
    descr->commandSize = 1;
    descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

    descr = &tcpReplies[REPLY_MEASURED_TORQUE];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0x9a;
    descr->commandSize = 1;
    descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpReplies[REPLY_DESIRED_POSITION];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0x9b;
    descr->commandSize = 1;
    descr->payloadSize = 4;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpReplies[REPLY_DESIRED_VELOCITY];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0x9c;
    descr->commandSize = 1;
    descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpReplies[REPLY_DESIRED_TORQUE];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0x9d;
    descr->commandSize = 1;
    descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpReplies[REPLY_ACCEL];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0x9e;
    descr->commandSize = 1;
    descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpReplies[REPLY_MIN_POSITION];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0xa0;
    descr->commandSize = 1;
    descr->payloadSize = 4;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

    descr = &tcpReplies[REPLY_MAX_POSITION];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0xa1;
    descr->commandSize = 1;
    descr->payloadSize = 4;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

    descr = &tcpReplies[REPLY_MAX_VELOCITY];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0xa2;
    descr->commandSize = 1;
    descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

    descr = &tcpReplies[REPLY_MIN_VELOCITY];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0xaf;
    descr->commandSize = 1;
    descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

    descr = &tcpReplies[REPLY_MAX_TORQUE];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0xa3;
    descr->commandSize = 1;
    descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpReplies[REPLY_PID_OFFSET];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0xa4;
    descr->commandSize = 1;
    descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

    descr = &tcpReplies[REPLY_CURRENT_LIMIT];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0xa5;
    descr->commandSize = 1;
    descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;
    
	descr = &tcpReplies[REPLY_BCAST_RATE];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0xa6;
    descr->commandSize = 1;
    descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpReplies[REPLY_BCAST_POLICY];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0xa7;
    descr->commandSize = 1;
    descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpReplies[REPLY_ENCODER_LINES];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0xa8;
    descr->commandSize = 1;
    descr->payloadSize = 4;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

    descr = &tcpReplies[REPLY_MOTOR_POLES];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0xa9;
    descr->commandSize = 1;
    descr->payloadSize = 1;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

    descr = &tcpReplies[REPLY_ANALOG_INPUTS];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0xaa;
    descr->commandSize = 1;
    descr->payloadSize = 1;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

    descr = &tcpReplies[REPLY_CALIBRATION_CURRENT];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0xae;
    descr->commandSize = 1;
    descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

    descr = &tcpReplies[REPLY_ABSOLUTE_ZERO];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0xc4;
    descr->commandSize = 1;
    descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

    descr = &tcpReplies[REPLY_TORQUE_ACCEL];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0xc5;
    descr->commandSize = 1;
    descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

    descr = &tcpReplies[REPLY_MOTOR_TYPE];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0xc6;
    descr->commandSize = 1;
    descr->payloadSize = 1;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;
	    
	descr = &tcpReplies[REPLY_TORQUE_ONOFF];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0xb6;
    descr->commandSize = 1;
    descr->payloadSize = 1;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpReplies[REPLY_MOTOR_CONFIG];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0xb7;
    descr->commandSize = 1;
    descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpReplies[REPLY_PD_LINK_GAINS];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0xb2;
    descr->commandSize = 1;
    descr->payloadSize = 8;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;
    
	descr = &tcpReplies[REPLY_MIN_ABS_POSITION];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0xCC;
    descr->commandSize = 1;
    descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpReplies[REPLY_MAX_ABS_POSITION];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0xCD;
    descr->commandSize = 1; 
    descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	///////////////////////////  FT sensor messaage ////////////////////
	descr = &tcpReplies[REPLY_CALIBRATION_OFFSETS];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0xce;
    descr->commandSize = 1;
    descr->payloadSize = 0x0c;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpReplies[REPLY_RESOLUTION];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0xcf;
    descr->commandSize = 1;
    descr->payloadSize = 1;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpReplies[REPLY_TEMP_FACTORS];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0xc2;
    descr->commandSize = 1;
    descr->payloadSize = 0x0c;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpReplies[REPLY_CAL_TEMP];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0xc8;
    descr->commandSize = 1;
    descr->payloadSize = 2;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpReplies[REPLY_CONVERSION_FACTORS];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0xc3;
    descr->commandSize = 1;
    descr->payloadSize = 0x07;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpReplies[REPLY_AVARAGE_SAMPLES];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0xAC;
    descr->commandSize = 1;
    descr->payloadSize = 1;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &tcpReplies[REPLY_MATRIX_ROW];
	descr->header  = HEADER_TCP_REPLY;
	descr->command[0]  = 0xAB;
    descr->commandSize = 1;
    descr->payloadSize = 19;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;


    /* UDP COMMANDS :  */
    // For UDP commands the payload size must be multiplied but the current number
	// of active boards, to get the actual size.
	descr = &udpCommands[GET_ACTIVE_BOARDS];
	descr->header  = HEADER_UDP_COMMAND;
	descr->command[0]  = 0x02;
	descr->command[1]  = 0xff;
    descr->commandSize = 2;
    descr->payloadSize = 0;
	descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;
    
    descr = &udpCommands[SET_DESIRED_POSITION];
	descr->header  = HEADER_UDP_COMMAND;
	descr->command[0] = 0x5b;
    descr->commandSize = 1;
    descr->payloadSize = 4 * numOfBoards;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

    descr = &udpCommands[SET_DESIRED_STIFFDAMP];
	descr->header  = HEADER_UDP_COMMAND;
	descr->command[0] = 0x59;
    descr->commandSize = 1;
    descr->payloadSize = (unsigned char) (6 * numOfBoards);
    descr->totSize     =(unsigned char) (getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize);

	descr = &udpCommands[SET_DESIRED_VELOCITY];
	descr->header  = HEADER_UDP_COMMAND;
	descr->command[0] = 0x5c;
    descr->commandSize = 1;
    descr->payloadSize = 2 * numOfBoards;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

    descr = &udpCommands[SET_DESIRED_TORQUE];
	descr->header  = HEADER_UDP_COMMAND;
	descr->command[0] = 0x5d;
    descr->commandSize = 1;
    descr->payloadSize = 2 * numOfBoards;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &udpCommands[SET_PID_OFFSET];
	descr->header  = HEADER_UDP_COMMAND;
	descr->command[0] = 0x64;
    descr->commandSize = 1;
    descr->payloadSize = 2 * numOfBoards;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

	descr = &udpCommands[POSITION_MOVE];
	descr->header  = HEADER_UDP_COMMAND;
	descr->command[0] = 0x36;
    descr->commandSize = 1;
    descr->payloadSize = numOfBoards;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

    descr = &udpCommands[VELOCITY_MOVE];
	descr->header  = HEADER_UDP_COMMAND;
	descr->command[0] = 0x37;
    descr->commandSize = 1;
    descr->payloadSize = numOfBoards;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

    descr = &udpCommands[TORQUE_MOVE];
	descr->header  = HEADER_UDP_COMMAND;
	descr->command[0] = 0x38;
    descr->commandSize = 1;
    descr->payloadSize = numOfBoards;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;
	

	/* UDP REPLIES :  */
	descr = &udpReplies[REPLY_ACTIVE_BOARDS];
	descr->header  = HEADER_UDP_REPLY;
	descr->command[0] = 0x82;
    descr->commandSize = 1;
    descr->payloadSize = 6;
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

    
    /* UDP BROADCASTS :  */
    descr = &udpBroadcasts[BCAST_DATA_PACKET];
	descr->header  = 0xfd;
	descr->command[0] = 0xbb;
    descr->commandSize = 1;
    descr->payloadSize = 2;//??? TODO
    descr->totSize     = getStaticFieldsSize(descr->payloadSize) + descr->commandSize + descr->payloadSize;

    
    
    //int size = sizeof(Packet);
    //char* blankPacket = (char*)malloc(size);
    //memset(blankPacket, '\0', size);
    //if( memcmp(blankPacket, &tcpCommands[TCP_COMMANDS_COUNT-1], size) == 0 ) {
		//fprintf(stderr, "Not all the tcp commands seem initialized. Check the source file\n");
		//exit(-1);
	//}
};

int getPayloadOffset(const PckDescr* descr) {
	return (fLength_header + fLength_payloadSize + descr->commandSize);
}

const PckDescr* getDescriptor_TCPCmd(TCP_command which){
	return &(tcpCommands[which]);
}
const PckDescr* getDescriptor_UDPCmd(UDP_command which){
	return &(udpCommands[which]);
}
const PckDescr* getDescriptor_TCPRep(TCP_reply which){
	return &(tcpReplies[which]);
}





static int fill(CharBuff* toBeFilled,  PckDescr* descr, char* payload) {
	int offset;

	if((toBeFilled == NULL)  ||  (payload == NULL  && descr->payloadSize > 0)) {
		return -1;
	}
	
	toBeFilled-> size = descr->totSize;
	toBeFilled-> content[0] = descr->header;
	toBeFilled-> content[fLength_header] = descr->payloadSize;
	//Copies the bytes of the command, after the header and the payload size:
	offset = fLength_header + fLength_payloadSize;
	memcpy(toBeFilled->content + offset, descr->command, descr->commandSize);
	if(descr->payloadSize > 0) {
		//Copies the actual content (payload):
		offset += descr->commandSize;
		memcpy(toBeFilled->content + offset, payload, descr->payloadSize);
		setChecksum(toBeFilled);
	}
	return 0;
}

int buildTCPCommand(CharBuff* toBeFilled, TCP_command which, char* payload) {
	if(which < 0  ||  which >= TCP_COMMANDS_COUNT) {
		return -1;
	}
	return fill(toBeFilled, &tcpCommands[which], payload);
}

int buildUDPCommand(CharBuff* toBeFilled,  UDP_command which, char* payload) {
	if(which < 0  ||  which >= UDP_COMMANDS_COUNT) {
		return -1;
	}
	return fill(toBeFilled, &udpCommands[which], payload);
}

int buildTCPReply(CharBuff* toBeFilled,  TCP_reply which, char* payload) {
	if(which < 0  ||  which >= TCP_REPLIES_COUNT) {
		return -1;
	}
	return fill(toBeFilled, &tcpReplies[which], payload);
}

int buildUDPReply(CharBuff* toBeFilled,  UDP_reply which, char* payload) {
	if(which < 0  ||  which >= UDP_REPLIES_COUNT) {
		return -1;
	}
	return fill(toBeFilled, &udpReplies[which], payload);
}

int buildUDPBroadcast(CharBuff* toBeFilled,  UDP_broadcast which, char* payload) {
	if(which < 0  ||  which >= UDP_BROADCASTS_COUNT) {
		return -1;
	}
	return fill(toBeFilled, &udpBroadcasts[which], payload);
}






