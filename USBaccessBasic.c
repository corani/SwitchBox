// Basic class implementation for access to USB HID devices
//


#define HID_MAX_USAGES 1024		// see /usr/src/linux/drivers/usb/input/hid.h

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <usb.h>
#include <linux/hiddev.h>

#include "USBaccessBasic.h"

const int maxHID = 128 ;
cwSUSBdata data[128] ;

void
cwInitDevice() {
	int h ;

	for (h=0 ; h < maxHID ; h++)
		data[h].handle = INVALID_HANDLE_VALUE ;
	}

void
cwCloseDevice() {
	int h ;
	for (h=0 ; h < maxHID  ; h++) {
		if (data[h].handle != INVALID_HANDLE_VALUE) {
			close(data[h].handle) ;
			data[h].handle = INVALID_HANDLE_VALUE ;
			}
		}
	}


// returns number of found devices
int
cwOpenDevice() {
	int ok = 1 ;	
	int i, h ;
	int handleCount = 0 ;

	for (h=0 ; h < handleCount ; h++) {
		if (data[h].handle != INVALID_HANDLE_VALUE) {
			close(data[h].handle) ;
			data[h].handle = INVALID_HANDLE_VALUE ;
			}
		}

	for (i=0 ; i < 16 ; i++) {		// Linux supports up to 16 HID devices
		struct hiddev_devinfo dinfo ;
		struct hiddev_report_info rinfo ;
		const char hiddevname[] = "/dev/usb/hiddev" ;
		char devname[32] ;
		int tryNextOne = 0 ;	// if 1 skip current device and try the next one

		sprintf(devname, "%s%d", hiddevname, i) ;
		data[handleCount].handle = open(devname, O_RDWR) ;
		if (data[handleCount].handle >= 0) {
			ok = ioctl(data[handleCount].handle, HIDIOCGDEVINFO, (void *)&dinfo) ;
			if (ok < 0)
				tryNextOne = 1 ;
			else if (dinfo.vendor != 0x0d50)
				tryNextOne = 1 ;

			if (ok >= 0 && !tryNextOne) {
				data[handleCount].gadgettype = (enum cwUSBtype_enum)dinfo.product ;
				data[handleCount].gadgetVersionNo = dinfo.version ;
				ok = ioctl(data[handleCount].handle, HIDIOCAPPLICATION, 0) ;
				if (ok == -1)
					tryNextOne = 1 ;
				else
					ok = 0 ;
				}

			if (ok >= 0 && !tryNextOne) {
				static char strbuf[261] ; // 256 + sizeof(int) + 1 ;
				*(int *) strbuf = 3 ;
				ok = ioctl(data[handleCount].handle, HIDIOCGSTRING, (void *)&strbuf) ;
				if (ok < 0)
					tryNextOne = 1 ;
				else {
					int SerNum=0 ;
					char *s=strbuf+sizeof(int) ;
					int strInc = (s[1] == 0) ? 2 : 1 ;
					for ( ; *s ; s+=strInc) {	// unicode byte 2 == 0
						if (*s >= '0' && *s <= '9')
							SerNum = SerNum * 16 + *s - '0' ;
						else if (*s >= 'A' && *s <= 'F')
							SerNum = SerNum * 16 + *s - 'A' + 10 ;
						}
					data[handleCount].SerialNumber = SerNum ;
					data[handleCount].report_type = HID_REPORT_ID_FIRST ;
					}
				}

			if (ok >= 0 && !tryNextOne)
				handleCount++ ;
			else {	// not ok - close handle
				close(data[handleCount].handle) ;
				data[handleCount].handle = INVALID_HANDLE_VALUE ;
				}

			}
		}

	return handleCount ;
	}

// try to find disconnected devices - returns true if succeeded
int
cwRecover(int devNum) {
	int ok = 1 ;	
	int i, h ;
	int reconnectOk = 0 ;
	cwSUSBdata USBdata ;

	if (data[devNum].handle != INVALID_HANDLE_VALUE) {
		close(data[devNum].handle) ;
		data[devNum].handle = INVALID_HANDLE_VALUE ;
		}

	for (i=0 ; i < 16 ; i++) {		// Linux supports up to 16 HID devices
		struct hiddev_devinfo dinfo ;
		struct hiddev_report_info rinfo ;
		const char hiddevname[] = "/dev/usb/hiddev" ;
		char devname[32] ;
		int tryNextOne = 0 ;	// if 1 skip current device and try the next one

		sprintf(devname, "%s%d", hiddevname, i) ;
		USBdata.handle = open(devname, O_RDWR) ;
		if (USBdata.handle >= 0) {
			ok = ioctl(USBdata.handle, HIDIOCGDEVINFO, (void *)&dinfo) ;
			if (ok < 0)
				tryNextOne = 1 ;
			else if (dinfo.vendor != 0x0d50)
				tryNextOne = 1 ;

			if (ok >= 0 && !tryNextOne) {
				USBdata.gadgettype = (enum cwUSBtype_enum)dinfo.product ;
				USBdata.gadgetVersionNo = dinfo.version ;
				ok = ioctl(USBdata.handle, HIDIOCAPPLICATION, 0) ;
				if (ok == -1)
					tryNextOne = 1 ;
				else
					ok = 0 ;
				}

			if (ok >= 0 && !tryNextOne) {
				static char strbuf[261] ; // 256 + sizeof(int) + 1 ;
				*(int *) strbuf = 3 ;
				ok = ioctl(USBdata.handle, HIDIOCGSTRING, (void *)&strbuf) ;
				if (ok >= 0) {
					int SerNum=0 ;
					char *s=strbuf+sizeof(int) ;
					int strInc = (s[1] == 0) ? 2 : 1 ;
					for ( ; *s ; s+=strInc) {	// unicode byte 2 == 0
						if (*s >= '0' && *s <= '9')
							SerNum = SerNum * 16 + *s - '0' ;
						else if (*s >= 'A' && *s <= 'F')
							SerNum = SerNum * 16 + *s - 'A' + 10 ;
						}
					USBdata.SerialNumber = SerNum ;
					if (		USBdata.gadgettype == data[devNum].gadgettype
							 && USBdata.gadgetVersionNo == data[devNum].gadgetVersionNo 
							 && USBdata.SerialNumber == data[devNum].SerialNumber) {
						data[devNum].handle = USBdata.handle ;
						data[devNum].report_type = HID_REPORT_ID_FIRST ;
						reconnectOk = 1;
						break ;
						}
					}
				}

			close(USBdata.handle) ;
			USBdata.handle = INVALID_HANDLE_VALUE ;
			}
		}

	return reconnectOk ;
	}

// returns 1 if ok or 0 in case of an error
int		
cwGetValue(int deviceNo, int UsagePage, int Usage, unsigned char *buf, int bufsize) {
	// UsagePage and Usage needed for win32
	struct hiddev_field_info finfo ;
	struct hiddev_usage_ref uref ;
	struct hiddev_report_info rinfo ;
	int ok = 1 ;

	if (deviceNo < 0 || deviceNo >= maxHID || data[deviceNo].handle == INVALID_HANDLE_VALUE)
		return 0 ;		// out of range

	if (data[deviceNo].report_type == HID_REPORT_ID_FIRST) {	// doing this the first time
		ok = ioctl(data[deviceNo].handle, HIDIOCAPPLICATION, 0) ;
		if (ok != -1) {
			rinfo.report_id = 0 ;
			rinfo.report_type = HID_REPORT_TYPE_INPUT ;
			ok = ioctl(data[deviceNo].handle, HIDIOCGREPORT, (void *)&rinfo) ;
			}

		if (ok >= 0) {
			rinfo.report_id = HID_REPORT_ID_FIRST ;		// only one report
			rinfo.report_type = HID_REPORT_TYPE_INPUT ;
			ok = ioctl(data[deviceNo].handle, HIDIOCGREPORTINFO, (void *)&rinfo) ;
			}
		if (ok >= 0) {
			finfo.report_type = rinfo.report_type ;
			finfo.report_id = rinfo.report_id ;
			finfo.field_index = 0 ;
			ok = ioctl(data[deviceNo].handle, HIDIOCGFIELDINFO, (void *)&finfo) ;
			}
		if (ok >= 0) {
			uref.report_type = finfo.report_type ;
			uref.report_id = finfo.report_id ;
			uref.field_index = 0 ;
			uref.usage_index = 0 ;
			ok = ioctl(data[deviceNo].handle, HIDIOCGUCODE, (void *)&uref) ;
			}
		if (ok >= 0 && finfo.maxusage != bufsize) {
			// printf("bufsize mismatch maxusage=%d\n", finfo.maxusage) ;
			ok = -1 ;
			}
		if (ok >= 0)
			data[deviceNo].report_type = rinfo.report_type ;
		}
	else {
		uref.report_type = data[deviceNo].report_type ;
		uref.report_id = 0 ;
		uref.field_index = 0 ;
		uref.usage_index = 0 ;
		}
	if (ok >= 0) {
		int u ;
		for (u=0 ; u < bufsize ; u++) {
			uref.usage_index = u ;
			ok = ioctl(data[deviceNo].handle, HIDIOCGUSAGE, (void *)&uref) ;
			if (ok < 0) {
				// perror("HIDIOCGUSAGE failed - ") ;
				break ;
				}
			buf[u] = uref.value & 0xff ;
			}
		}
	return (ok >= 0) ? 1 : 0 ;
	}


int 
cwSetValue(int deviceNo, int UsagePage, int Usage, unsigned char *buf, int bufsize) {
	// UsagePage and Usage needed for win32
	struct hiddev_report_info rinfo ;
	struct hiddev_field_info finfo ;
	struct hiddev_usage_ref uref ;
	int ok  = 0 ;

	if (deviceNo < 0 || deviceNo >= maxHID || data[deviceNo].handle == INVALID_HANDLE_VALUE)
		return 0 ;		// out of range

	rinfo.report_id = HID_REPORT_ID_FIRST ;		// only one report
	rinfo.report_type = HID_REPORT_TYPE_OUTPUT ;
	ok = ioctl(data[deviceNo].handle, HIDIOCGREPORTINFO, (void *)&rinfo) ;
	if (ok >= 0) {
		finfo.report_type = rinfo.report_type ;
		finfo.report_id = rinfo.report_id ;
		finfo.field_index = 0 ;
		ok = ioctl(data[deviceNo].handle, HIDIOCGFIELDINFO, (void *)&finfo) ;
		}
	if (ok >= 0) {
		uref.report_type = finfo.report_type ;
		uref.report_id = finfo.report_id ;
		uref.field_index = 0 ;
		uref.usage_index = 0 ;
		ok = ioctl(data[deviceNo].handle, HIDIOCGUCODE, (void *)&uref) ;
		}
	if (ok >= 0 && finfo.maxusage != bufsize) {
		// printf("bufsize mismatch maxusage=%d\n", finfo.maxusage) ;
		ok = -1 ;
		}
	if (ok >= 0) {
		int u ;
		for (u=0 ; u < finfo.maxusage ; u++) {
			uref.value = buf[u] ;
			uref.usage_index = u ;
			ok = ioctl(data[deviceNo].handle, HIDIOCSUSAGE, (void *)&uref) ;
			if (ok < 0)
				break ;
			}
		}
	if (ok >= 0) {
		ok = ioctl(data[deviceNo].handle, HIDIOCSREPORT, (void *)&finfo) ;
		}

	return (ok >= 0) ? 1 : 0 ;
	}

unsigned long int
cwGetHandle(int deviceNo) { 
	unsigned long int rval = INVALID_HANDLE_VALUE ;

	if (deviceNo >= 0 && deviceNo < maxHID)
		rval = data[deviceNo].handle ;

	return rval ; 
	}

int
cwGetVersion(int deviceNo) { 
	int rval ;

	if (deviceNo < 0 || deviceNo >= maxHID || data[deviceNo].handle == INVALID_HANDLE_VALUE)
		rval = -1 ;
	else
		rval = data[deviceNo].gadgetVersionNo ;

	return rval ; 
	}

int
cwGetSerialNumber(int deviceNo) { 
	int rval ;

	if (deviceNo < 0 || deviceNo >= maxHID || data[deviceNo].handle == INVALID_HANDLE_VALUE)
		rval = -1 ;
	else
		rval = data[deviceNo].SerialNumber ;

	return rval ; 
	}

enum cwUSBtype_enum
cwGetUSBType(int deviceNo) { 
	enum cwUSBtype_enum rval ;

	if (deviceNo < 0 || deviceNo >= maxHID || data[deviceNo].handle == INVALID_HANDLE_VALUE)
		rval = ILLEGAL_DEVICE ;
	else
		rval = data[deviceNo].gadgettype ;

	return rval ; 
	}
