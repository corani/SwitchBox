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
int CUSBaccess::OpenDevices()
{
	return cwOpenDevice();
}

int CUSBaccess::Recover(int devNum)
{
	return cwRecover(devNum);
}

// return true if ok, else false
int CUSBaccess::CloseDevices()
{
	cwCloseDevice();

	return 1;
}

Switchbox *CUSBaccess::GetSwitchbox(int deviceNo) {
    return new Switchbox(this, deviceNo);
}

int Switchbox::GetVersion()
{
	return cwGetVersion(this->id);
}

int Switchbox::GetSerialNumber()
{
	return cwGetSerialNumber(this->id);
}

// returns 1 if ok or 0 in case of an error
int Switchbox::GetValue(unsigned char *buf, int bufsize)
{
	return cwGetValue(this->id, buf, bufsize);
}


int Switchbox::SetValue(unsigned char *buf, int bufsize)
{
	return cwSetValue(this->id, buf, bufsize);
}

int Switchbox::SetLED(enum LED_IDs Led, int value)
{
	unsigned char s[3];

    s[0] = 0;
	s[1] = Led;
	s[2] = value;
	return this->SetValue(s, 3);
}

Switchbox::STATE Switchbox::SetSwitch(int secure, enum STATE state)
{
    if (secure) {
        for (int tryCnt = 0; tryCnt < 5; tryCnt++) {
            usleep(500 * 1000);
            STATE temp = this->GetSwitch();
            if (temp == state) {
                return state;
            }
            this->SetSwitch(0, state);
        }
    } else {
	    unsigned char s[3];
	    Switchbox::STATE rval = Switchbox::ERR;
	    int version = cwGetVersion(this->id);

	    s[0] = 0;
	    s[1] = SWITCH_0;
	    if (version < 4) {
	    	// old version do not invert
		    s[2] = (state == Switchbox::ON ? 0 : 1);
		} else {
		    s[2] = (state == Switchbox::ON ? 1 : 0);
		}

	    rval = SetValue(s, 3) ? Switchbox::ON : Switchbox::OFF;
	    if (rval == Switchbox::ON) {			// set LED for first switch
		    if (state == Switchbox::ON) {
			    SetLED(LED_0, 0);	// USB Switch will invert LED
			    SetLED(LED_1, 15);
		    } else {
			    SetLED(LED_0, 15);
			    SetLED(LED_1, 0);
		    }
	    }

	    return rval;
	}
}

Switchbox::STATE Switchbox::GetSwitch()
{
	const int bufSize = 6;
	unsigned char buf[bufSize];
	Switchbox::STATE res = Switchbox::ERR;
	int version = cwGetVersion(this->id);
	int mask = 1;

	if (!GetValue(buf, bufSize))
	    return Switchbox::ERR;
	else if (version >= 10)
		return (buf[0] & mask) ? Switchbox::ON : Switchbox::OFF;
	else if (version >= 4)
		return (buf[2] & mask) ? Switchbox::ON : Switchbox::OFF;
    else
		return (buf[2] & mask) ? Switchbox::OFF : Switchbox::ON;
}

