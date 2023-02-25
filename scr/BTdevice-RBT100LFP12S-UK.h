#pragma once
#include "BTwrap.h"

class BTdeviceRBT100LFP12SUK{
//device parameters
public:
	double cell1 = 0.0;	//V
	double cell2 = 0.0;
	double cell3 = 0.0;
	double cell4 = 0.0;
	double temp1 = 0.0;	//deg. C
	double temp2 = 0.0;
	double current = 0.0;	//A
	double voltage = 0.0;	//V
	double charge = 0.0;	//%

private:
	char output[BTshell_max_output_buffer] = { 0 };

public:
	//run the preset gatttool commands for the device
	bool gatttool_preset_command(BTwrap & bt) {
		//clear output if being called again
		for (int l = 0; l < BTshell_max_output_buffer; l++) {
			output[l] = 0;
		}

		//move address gen to a class, which contains the cmd[17] var and the whole thing then can be passed to gatttool_write_req
		char cmd1[17] = { 0 };
		char cmd2[17] = { 0 };
		char cmd3[17] = { 0 };
		char cmd4[17] = { 0 };
		//address_gen (cmd1, 0x3003, 0x1388, 0x0022);
		address_gen(cmd1, 0x3003, 0x1389, 0x0004);	//cell voltages
		address_gen(cmd2, 0x3003, 0x139a, 0x0002);	//temperatures
		address_gen(cmd3, 0x3003, 0x13b2, 0x0002);	//current, voltage
		address_gen(cmd4, 0x3003, 0x13b5, 0x0001);	//% charge

		//stop listening (allows the buffer on the device to fill)
		bool test_connection = bt.gatttool_write_req("0x002f", "0000", NULL);
		if (!test_connection) {
			std::cout << "unable to connect" << std::endl;
			return false;
		}

		//send commands to process
		bt.gatttool_write_req("0x001d", cmd1);
		bt.gatttool_write_req("0x001d", cmd2);
		bt.gatttool_write_req("0x001d", cmd3);
		bt.gatttool_write_req("0x001d", cmd4);

		//listen to the device output buffer
		bt.gatttool_write_req("0x002f", "0100", output);

		//reset listening to not listening, in case it has any effect on other devices wanting to talk to it...?
		bt.gatttool_write_req("0x002f", "0000");

		return true;
	}

public:
	//parse and map the bytes from the output of the preset command to their relevent properties
	void update_params(BTwrap & bt) {
		uint8_t output_bytes[BTwrap_max_output_buffer] = { 0 };
		int chk_l = bt.parse(output, output_bytes);

		if (chk_l == 38) {
			cell1 = byte_to_word(output_bytes[3], output_bytes[4]) / 10.0;
			cell2 = byte_to_word(output_bytes[5], output_bytes[6]) / 10.0;
			cell3 = byte_to_word(output_bytes[7], output_bytes[8]) / 10.0;
			cell4 = byte_to_word(output_bytes[9], output_bytes[10]) / 10.0;

			temp1 = byte_to_word(output_bytes[16], output_bytes[17]) / 10.0;
			temp2 = byte_to_word(output_bytes[18], output_bytes[19]) / 10.0;

			current = ((int16_t)byte_to_word(output_bytes[25], output_bytes[26])) / 100.0;
			voltage = byte_to_word(output_bytes[27], output_bytes[28]) / 10.0;

			charge = byte_to_word(output_bytes[34], output_bytes[35]) / 1000.0;
		}
	}

public:
	//basic verbose output of properties
	void std_out_params() {
		std::cout << "cells: " << cell1 << "V, " << cell2 << "V, " << cell3 << "V, " << cell4 << "V" << std::endl;
		std::cout << "temps: " << temp1 << "C, " << temp2 << "C" << std::endl;
		std::cout << "voltage: " << voltage << "V" << std::endl;
		std::cout << "current: " << current << "A" << std::endl;
		std::cout << "charge: " << charge << "%" << std::endl;
	}
};
