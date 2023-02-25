#include <iostream>
#include "BTwrap.h"
#include "BTdevice-RBT100LFP12S-UK.h"

using namespace std;
int main() {
	//create bt output buffer
	BTwrap bt ("hci0", "60:98:66:FB:4C:9C");
	BTdeviceRBT100LFP12SUK device;

	//run the device preset commands, update the parameters and display
	device.gatttool_preset_command(bt);
	device.update_params(bt);
	device.std_out_params();

	return 0;
}
