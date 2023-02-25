#pragma once
#include "config.h"
#define BTshell_max_output_buffer 1024

	bool shell_cmd(const char* cmd, const char* type, char* output, bool listen) {
		//creates an instance of the shell, str. 
		//takes cli arguments, and "r" read / "w" write
		//based on : https://raspberry-projects.com/pi/programming-in-c/console/using-console-commands-in-code
#ifdef RPI
		FILE* str = popen(cmd, type);
#else
		FILE* str = _popen(cmd, type);
#endif
		if (!str) {
			return false;
		}

		//this is blocking, however calling timout xs before the shell command ensures it will return
		if (listen) {
			int len = 0;

			while (!feof(str)) {
				char buf[128] = { 0 };
				len += 128;

				if (fgets(buf, 128, str) != NULL && len < BTshell_max_output_buffer) {
					//copy cli (str) to output
					strcat(output, buf);	//can cause seg. fault if overflown, hence len < BTwrap_max_output_buffer
				}
			}
		}

#ifdef RPI
		if (pclose(str) == -1) {
			return false;
		}
#else
		if (_pclose(str) == -1) {
			return false;
		}
#endif
		return true;
	}

	bool shell_cmd(const char* cmd, const char* type) {
		return shell_cmd(cmd, type, NULL, false);
	}

	bool shell_cmd(const char* cmd, const char* type, char* output) {
		return shell_cmd(cmd, type, output, true);
	}
