// DLL class definitions for access to USB HID devices
//

// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the USBACCESS_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// USBACCESS_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.


#ifndef __USBACCESS_H__
#define __USBACCESS_H__

typedef int HANDLE ;
const int USBaccessVersion = 110 ;

class CUSBaccess {
	public:
		enum USBactions {		LEDs=0, EEwrite=1, EEread=2, Reset=3, KeepCalm=4, GetInfo=5,
								StartMeasuring=6,		// USB-Humidity
								RunPoint=10				// USB-Encoder
								} ;
		enum USBInfoType {		OnlineTime=1, OnlineCount=2, ManualTime=3, ManualCount=4 } ;
		enum LED_IDs {			LED_0=0, LED_1=1, LED_2=2, LED_3=3 } ;
		enum COUNTER_IDs {		COUNTER_0=0, COUNTER_1=1 } ;
		enum SWITCH_IDs {		SWITCH_0=0x10, SWITCH_1=0x11, SWITCH_2=0x12, SWITCH_3=0x13,
								SWITCH_4=0x14, SWITCH_5=0x15, SWITCH_6=0x16, SWITCH_7=0x17,
								SWITCH_8=0x18, SWITCH_9=0x19, SWITCH_10=0x1a, SWITCH_11=0x1b,
								SWITCH_12=0x1c, SWITCH_13=0x1d, SWITCH_14=0x1e, SWITCH_15=0x1f
								} ;
		enum USBtype_enum {		ILLEGAL_DEVICE=0,
								LED_DEVICE=0x01,
								WATCHDOG_DEVICE=0x05,
								AUTORESET_DEVICE=0x06,
								SWITCH1_DEVICE=0x08,
								SWITCH2_DEVICE=0x09, SWITCH3_DEVICE=0x0a, SWITCH4_DEVICE=0x0c,
								TEMPERATURE_DEVICE=0x10,
								TEMPERATURE2_DEVICE=0x11,
								TEMPERATURE5_DEVICE=0x15,
								HUMIDITY1_DEVICE=0x20,
								CONTACT00_DEVICE=0x30, CONTACT01_DEVICE=0x31, CONTACT02_DEVICE=0x32, CONTACT03_DEVICE=0x33,
								CONTACT04_DEVICE=0x34, CONTACT05_DEVICE=0x35, CONTACT06_DEVICE=0x36, CONTACT07_DEVICE=0x37,
								CONTACT08_DEVICE=0x38, CONTACT09_DEVICE=0x39, CONTACT10_DEVICE=0x3a, CONTACT11_DEVICE=0x3b,
								CONTACT12_DEVICE=0x3c, CONTACT13_DEVICE=0x3d, CONTACT14_DEVICE=0x3e, CONTACT15_DEVICE=0x3f,

								ENCODER01_DEVICE=0x80,
								BUTTON_NODEVICE=0x1000
								} ;
	public:
		CUSBaccess() ;
		virtual ~CUSBaccess() ;		// maybe used as base class

		virtual int			OpenDevice() ;			// returns number of found devices
		virtual int			CloseDevice() ;		// close all devices
		virtual int			Recover(int devNum) ;	// try to find disconnected devices, returns true if succeeded
		virtual HANDLE		GetHandle(int deviceNo) ;
		virtual int			GetValue(int deviceNo, unsigned char *buf, int bufsize) ;
		virtual int			SetValue(int deviceNo, unsigned char *buf, int bufsize) ;
		virtual int			SetLED(int deviceNo, enum LED_IDs Led, int value) ;	// value: 0=off 7=medium 15=highlight
		virtual int			SetSwitch(int deviceNo, enum SWITCH_IDs Switch, int On) ;	//	On: 0=off, 1=on
		virtual int			GetSwitch(int deviceNo, enum SWITCH_IDs Switch) ;			//	On: 0=off, 1=on, -1=error
		virtual int			GetSeqSwitch(int deviceNo, enum SWITCH_IDs Switch, int seqNum) ;		//	On: 0=off, 1=on, -1=error
		virtual int			GetSwitchConfig(int deviceNo, int *switchCount, int *buttonAvailable) ;
		virtual int			GetTemperature(int deviceNo, double *Temperature, int *timeID) ;
		virtual int			GetHumidity(int deviceNo, double *Humidity, int *timeID) ;
		virtual int			ResetDevice(int deviceNo) ;
		virtual int			StartDevice(int deviceNo) ;
		virtual int			CalmWatchdog(int deviceNo, int minutes, int minutes2restart) ;
		virtual int			GetVersion(int deviceNo) ;
		virtual int			GetUSBType(int deviceNo) ;
		virtual int			GetSerialNumber(int deviceNo) ;
		virtual int			GetDLLVersion() { return USBaccessVersion ; }
		virtual int			GetManualOnCount(int deviceNo) ;		// returns how often switch is manually turned on
		virtual int			GetManualOnTime(int deviceNo) ;			// returns how long (seconds) switch is manually turned on
		virtual int			GetOnlineOnCount(int deviceNo) ;		// returns how often switch is turned on by USB command
		virtual int			GetOnlineOnTime(int deviceNo) ;			// returns how long (seconds) switch is turned on by USB command
		virtual int			GetMultiSwitch(int deviceNo, unsigned long int *mask, unsigned long int *value, int seqNumber) ;
		virtual int			SetMultiSwitch(int deviceNo, unsigned long int value) ;
		virtual int			SetMultiConfig(int deviceNo, unsigned long int directions) ;
		virtual int			GetCounter(int deviceNo, enum COUNTER_IDs counter) ;	// return value of counter (0 or 1 for USB-IO16)
		virtual int			SyncDevice(int deviceNo, unsigned long int mask) ;
		virtual void		Sleep(int ms) { usleep(ms * 1000) ; }	// for Linux
	} ;

#endif // __USBACCESS_H__
