// Basic class implementation for access to USB HID devices
//

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "USBaccess.h"

extern "C" {
#include "USBaccessBasic.h"
};


CUSBaccess::CUSBaccess()
{
	cwInitDevice();
}

CUSBaccess::~CUSBaccess()
{
}

// returns number of found devices
int CUSBaccess::OpenDevice()
{
	return cwOpenDevice();
}

int CUSBaccess::Recover(int devNum)
{
	return cwRecover(devNum);
}

// return true if ok, else false
int CUSBaccess::CloseDevice()
{
	cwCloseDevice();

	return 1;
}

HANDLE CUSBaccess::GetHandle(int deviceNo)
{
	return cwGetHandle(deviceNo);
}

int CUSBaccess::GetVersion(int deviceNo)
{
	return cwGetVersion(deviceNo);
}

int CUSBaccess::GetUSBType(int deviceNo)
{
	return cwGetUSBType(deviceNo);
}

int CUSBaccess::GetSerialNumber(int deviceNo)
{
	return cwGetSerialNumber(deviceNo);
}

// returns 1 if ok or 0 in case of an error
int CUSBaccess::GetValue(int deviceNo, unsigned char *buf, int bufsize)
{
	return cwGetValue(deviceNo, buf, bufsize);
}


int CUSBaccess::SetValue(int deviceNo, unsigned char *buf, int bufsize)
{
	return cwSetValue(deviceNo, buf, bufsize);
}

int CUSBaccess::SetLED(int deviceNo, enum LED_IDs Led, int value)
{
	unsigned char s[3];

    s[0] = 0;
	s[1] = Led;
	s[2] = value;
	return SetValue(deviceNo, s, 3);
}

int CUSBaccess::SetSwitch(int deviceNo, enum SWITCH_IDs Switch, int On)
{
	unsigned char s[3];
	int rval = 0;
	int version = cwGetVersion(deviceNo);

	s[0] = 0;
	s[1] = Switch;
	if (version < 4)	// old version do not invert
		s[2] = !On;
	else
		s[2] = On;

	rval = SetValue(deviceNo, s, 3);
	if (rval && Switch == SWITCH_0) {			// set LED for first switch
		if (On) {
			SetLED(deviceNo, LED_0, 0);	// USB Switch will invert LED
			SetLED(deviceNo, LED_1, 15);
		} else {
			SetLED(deviceNo, LED_0, 15);
			SetLED(deviceNo, LED_1, 0);
		}
	}

	return rval;
}

// On 0=off, 1=on, -1=error
int CUSBaccess::GetSwitch(int deviceNo, enum SWITCH_IDs Switch)
{
	const int bufSize = 6;
	unsigned char buf[bufSize];
	int ok = 0;
	USBtype_enum devType = (USBtype_enum)cwGetUSBType(deviceNo);

	int version = cwGetVersion(deviceNo);

	if (GetValue(deviceNo, buf, bufSize)) {
		int mask = 1 << ((Switch - SWITCH_0) * 2);
		if (version >= 10)
			ok = (buf[0] & mask) ? 1 : 0;
        else	// old switch
            ok = (buf[2] & mask) ? 1 : 0;
    } else
		ok = -1;	// getvalue failed - may be disconnected

	if (ok >= 0 && version < 4)
		ok = !ok;

	return ok;
}

