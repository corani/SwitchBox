// Basic class definitions for access to USB HID devices
//
#ifndef __USBACCESSBASIC_H__
#define __USBACCESSBASIC_H__

#define INVALID_HANDLE_VALUE -1

enum cwUSBtype_enum {
    ILLEGAL_DEVICE      = 0x00,
	SWITCH1_DEVICE      = 0x08,
};

typedef struct {
	unsigned long int	handle ;
	int					gadgetVersionNo ;
	enum cwUSBtype_enum	gadgettype ;
	int					SerialNumber ;
	int					report_type ;
} cwSUSBdata;

void cwInitDevice();
int	cwOpenDevice();	// returns number of found devices
int cwRecover(int deviceNo);
void cwCloseDevice();
int	cwGetValue(int deviceNo, unsigned char *buf, int bufsize);
int	cwSetValue(int deviceNo, unsigned char *buf, int bufsize);
unsigned long int cwGetHandle(int deviceNo);
int	cwGetVersion(int deviceNo);
int	cwGetSerialNumber(int deviceNo);
enum cwUSBtype_enum	cwGetUSBType(int deviceNo);

#endif // __USBACCESS_H__
