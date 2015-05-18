# SwitchBox
Linux driver for <a href="http://www.antrax.de/site/Online-Shop/Home/Switchable-Socket/SwitchBox-USB::9.html" target="_BLANK">Antrax Datentechnik GmbH SwitchBox IP20</a>.

<img width="200" src="http://www.antrax.de/site/images/product_images/original_images/SwitchBox_USB.jpg" />

This is based on the original <a href="http://www.antrax.de/downloads/switchboxusb-linux.zip">Vendor Code</a>,  adds support for serial numbers - so you can control multiple SwitchBoxes connected to the same machine - and can be compiled on modern Linux systems.

## UDEV permissions
To automatically give users access to control the USB SwitchBox, create the following udev rule:

1. In /etc/udev/rules.d/10-local.rules add:
> SUBSYSTEM=="usbmisc", ATTRS{idVendor}=="0d50", ATTRS{idProduct}=="0008", GROUP="users", MODE="0660"

2. Restart udevd:
> sudo /etc/init.d/udev restart

3. (Unplug and) plug in your SwitchBox
 
## Building
1. Clone this git repo, or <a href="https://github.com/corani/SwitchBox/archive/master.zip">download the ZIP archive</a>.
2. Run "make" in the folder containing the code
 
## Running
Get help by executing "USBswitch -h"

If you've got just one SwitchBox connected, you can switch it by executing "USBswitch 0" for *off* or "USBswitch 1" for *on*.

If you've got multiple SwitchBoxes connected, you'll need to specify the serial number using the "-n <serialNumber>" switch. One way to find the serial number is to run "USBswitch -d" to enumerate all SwitchBoxes and print debug information.
