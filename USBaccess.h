#ifndef __USBACCESS_H__
#define __USBACCESS_H__

typedef int HANDLE;
const int USBaccessVersion = 110;

class Switchbox;

class CUSBaccess {
	public:
		CUSBaccess();
		virtual ~CUSBaccess();		// maybe used as base class

		virtual int			OpenDevices();			// returns number of found devices
		virtual int			CloseDevices();		    // close all devices
		virtual int			Recover(int devNum);	// try to find disconnected devices, returns true if succeeded
		Switchbox *         GetSwitchbox(int id);
};

class Switchbox {
private:
    CUSBaccess *usb;
    int id;
public:
	enum LED_IDs {
        LED_0 = 0,
        LED_1 = 1,
    };
    enum STATE {
        ERR = -1,
        OFF =  0,
        ON  =  1,
    };
	enum SWITCH_IDs {
        SWITCH_0  = 0x10,
	};
public:
    Switchbox(CUSBaccess *usb, int id) : usb(usb), id(id) {}
    ~Switchbox() {}
    
    STATE SetSwitch(int secure, enum STATE state);
	STATE GetSwitch();
	int	GetVersion();
	int GetSerialNumber();
private:
	int	GetValue(unsigned char *buf, int bufsize);
	int	SetValue(unsigned char *buf, int bufsize);
	int	SetLED(enum LED_IDs Led, int value);	// value: 0=off 7=medium 15=highlight
};
#endif // __USBACCESS_H__
