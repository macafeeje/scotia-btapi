#pragma once
#include <stdio.h>
#include <iostream>
#include <cstring>
#include "BTshell.h"
#include "address-gen.h"
#include "config.h"

#define BTwrap_max_output_buffer 128
//a cpp wrapper for the gatttool shell command gatttool-write-req | for interfacing with a BLE device
class BTwrap {

public:
	char mac_addr[18] = { 0 };
	char interface_name[128] = { 0 };
	char default_timeout[3] = "1s";
	
public:
	BTwrap(const char *interface_nm, const char *mac_address) {
		set_mac_addr(mac_address);
		set_interface_name(interface_nm);
	}

	BTwrap() {

	}

public:
	bool set_mac_addr(const char* address) {
		int len = strlen(address);
		if (len != 17){
			return false;
		}
		for (int l = 0; l < 18; l++) {
			mac_addr[l] = address[l];
		}
		return true;
	}

public:
	bool set_interface_name(const char* name) {
		int len = strlen(name);
		if (len < 128) {
			for (int l = 0; l < len; l++) {
				interface_name[l] = name[l];
			}
			return true;
		}
		return false;
	}
	
public:
	//handle address, value to write, output string /w --listen
	bool gatttool_write_req(const char* handle, const char* value, char* output) {
		return gatttool_write_req(interface_name, mac_addr, handle, value, true, output, default_timeout);
	}
	//handle address, value to write, no --listen
	bool gatttool_write_req(const char* handle, const char* value) {
		return gatttool_write_req(interface_name, mac_addr, handle, value, false, NULL, default_timeout);
	}

private:
	bool gatttool_write_req(const char* interface, const char* mac_address, const char* handle, const char* value, bool listen, char* output, const char* timeout) {
		//compile gatttool command from args
		char buf[256] = { 0 };
		strcat(buf, "timeout ");
		strcat(buf, timeout);
		strcat(buf, " gatttool -i ");
		strcat(buf, interface);
		strcat(buf, " -b ");
		strcat(buf, mac_address);
		strcat(buf, " --char-write-req -a ");
		strcat(buf, handle);
		strcat(buf, " -n ");
		strcat(buf, value);
		if (listen) {
			strcat(buf, " --listen");
		}

		//std::cout << buf << std::endl;

		//run command in the shell
		if (listen && output != NULL) {
			if (!shell_cmd(buf, "r", output)) {
				return false;
			}
			if (output[0] == 0) {
				return false;
			}
		}
		else {
			if (!shell_cmd(buf, "r")) {
				return false;
			}
		}
		return true;
	}

public:
	int parse(const char* str, uint8_t* output_bytes) {
		int len = strlen(str);

		//parses for ':', space, then reads two characters to and converts it to a byte
		//then skips a space, and if there's no new line/return char (i.e, new output line), it repeats
		//to populate output bytes

		int l = 0;
		int opt_l = 0;
		bool start = false;
		do {
			if (start) {
				if (l < len - 1) {
					output_bytes[opt_l] = char_to_byte(str[l], str[l + 1]);
					opt_l++;
					l = l + 2;
					if (l < len - 2) {
						if (str[l + 1] == '\n') {
							start = false;
						}
					}
				}
				else {
					break;
				}
			}
			else {
				if (str[l] == ':') {
					start = true;
					l++;
				}
			}
			l++;
		} while (l < len);

		//hex dump
		/*std::cout << std::endl << std::endl;
		for (int l = 0; l < opt_l; l++) {
			std::cout << std::hex << (int)output[l] << " ";
		}std::cout << std::endl;
		std::cout << std::dec << opt_l << std::endl;*/

		return opt_l;
	}
};
