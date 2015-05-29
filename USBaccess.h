#ifndef __USBACCESS_H__
#define __USBACCESS_H__

typedef int HANDLE;
const int USBaccessVersion = 110;

class CUSBaccess {
	public:
		enum LED_IDs {
            LED_0 = 0,
            LED_1 = 1,
        };
		enum SWITCH_IDs {
            SWITCH_0  = 0x10,
		};
		enum USBtype_enum {
            ILLEGAL_DEVICE = 0x00,
			SWITCH1_DEVICE = 0x08,
		};
	public:
		CUSBaccess();
		virtual ~CUSBaccess();		// maybe used as base class

		virtual int			OpenDevice();			// returns number of found devices
		virtual int			CloseDevice();		    // close all devices
		virtual int			Recover(int devNum);	// try to find disconnected devices, returns true if succeeded
		virtual HANDLE		GetHandle(int deviceNo);
		virtual int			GetValue(int deviceNo, unsigned char *buf, int bufsize);
		virtual int			SetValue(int deviceNo, unsigned char *buf, int bufsize);
		virtual int			SetLED(int deviceNo, enum LED_IDs Led, int value);	// value: 0=off 7=medium 15=highlight
		virtual int			SetSwitch(int deviceNo, enum SWITCH_IDs Switch, int On);	//	On: 0=off, 1=on
		virtual int			GetSwitch(int deviceNo, enum SWITCH_IDs Switch);			//	On: 0=off, 1=on, -1=error
		virtual int			GetVersion(int deviceNo);
		virtual int			GetUSBType(int deviceNo);
		virtual int			GetSerialNumber(int deviceNo);
};

#endif // __USBACCESS_H__
