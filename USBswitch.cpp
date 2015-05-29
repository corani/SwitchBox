/* USBswitch [-n device] [0 | 1] [-d]
 *           -n device   use device with this serial number
 *           0 | 1       turns switch off(0) or on(1)
 *           -d          print debug infos
 *           -s          secure switching - wait and ask if switching was done
 *           -r          read the current setting
 *           -v          print version
 *           -h          print command usage
 *
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "USBaccess.h"

int main(int argc, char* argv[])
{
    CUSBaccess CWusb;
    int debug = 0;
    int state = -1;     // 0=off, 1=on
    int doRead = 0;
    int secureSwitching = 0;
    int turnSwitch = -1;    // 0=off, 1=on
    int printVersion = 0;
    int printHelp = 0;
    int serialNumber = -1;
    bool found = false;
    char *progName = *argv;
    int ok = 1;
    static char *versionString = (char *) "2.0";

    for (argc--, argv++; argc > 0 && ok; argc--, argv++) {
        if (argv[0][0] == '0') {
            turnSwitch = 0;
        } else if (argv[0][0] == '1') {
            turnSwitch = 1;
        } else if (argv[0][0] == '-') {
            switch (argv[0][1]) {
            case 'd':
            case 'D':
                debug = 1;
                break;
            case 's':
            case 'S':
                secureSwitching = 1;
                break;
            case 'r':
            case 'R':
                doRead = 1;
                break;
            case 'v':
            case 'V':
                printVersion = 1;
                break;
            case 'h':
            case 'H':
                printHelp = 1;
                break;
            case 'n':
            case 'N':
                if (argc == 1) {
                    printf("missing serial number %s\n", *argv);
                    ok = 0;
                    break;
                }
                argc--;
                argv++;
                serialNumber = atoi(argv[0]);
                break;
            default:
                printf("illegal argument %s\n", *argv);
                ok = 0;
                break;
            }
        } else {
            printf("illegal argument %s\n", *argv);
            ok = 0;
        }
    }

    if (!ok) {
        return -1;
    }

    if (printHelp) {
        printf("Usage: %s [-n device] [0 | 1] [-d]\n", progName);
        printf("       -n device   use device with this serial number\n");
        printf("       0 | 1       turns switch off(0) or on(1)\n");
        printf("       -d          print debug infos\n");
        printf("       -s          secure switching - wait and ask if switching was done\n");
        printf("       -r          read the current setting\n");
        printf("       -v          print version\n");
        printf("       -h          print command usage\n");
    }

    if (printVersion) {
        printf("%s version %s\n", progName, versionString);
    }

    int USBcount = CWusb.OpenDevice();
    if (debug) {
        printf("OpenDevice found %d devices\n", USBcount);
    }

    if (USBcount > 0 && serialNumber == -1 && turnSwitch != -1) {
        printf("Found %d devices, specify the serial number with -n\n", USBcount);
        found = true;
        USBcount = -1;
    }

    for (int devID = 0; devID < USBcount; devID++) {
        if (debug) {
            printf("Device %d: Type=%d, Version=%d, SerNum=%d\n",
                devID,
                CWusb.GetUSBType(devID),
                CWusb.GetVersion(devID),
                CWusb.GetSerialNumber(devID)
            );
        }

        if (CWusb.GetUSBType(devID) != CUSBaccess::SWITCH1_DEVICE) {
            continue;
        }

        // A serial number was specified, but it's not this one
        if (serialNumber >= 0 && serialNumber != CWusb.GetSerialNumber(devID)) {
            continue;
        }

        found = true;

        if (debug) {
            printf("old switch setting = %d\n",
                CWusb.GetSwitch(devID, CUSBaccess::SWITCH_0)
            );
        }

        state = turnSwitch;
        if (turnSwitch == 0 || turnSwitch == 1) {
            ok = CWusb.SetSwitch(devID, CUSBaccess::SWITCH_0, turnSwitch);
        }

        if (!ok) {
            printf("USBswitch cannot be reached\n");
            state = -1;
            break;
        }

        if (secureSwitching && turnSwitch >= 0) {
            for (int tryCnt = 0; tryCnt < 5; tryCnt++) {
                usleep(500 * 1000);
                state = CWusb.GetSwitch(devID, CUSBaccess::SWITCH_0);
                if (turnSwitch == state) {
                    break;
                }
                if (debug) {
                    printf("retry switch!\n");
                }
                CWusb.SetSwitch(devID, CUSBaccess::SWITCH_0, turnSwitch);
            }
        }

        if (doRead) {
            printf("%d: %d\n",
                CWusb.GetSerialNumber(devID),
                CWusb.GetSwitch(devID, CUSBaccess::SWITCH_0)
            );
        }
    }

    if (USBcount == 0 || !found) {
        printf("USBswitch not found\n");
    }

    CWusb.CloseDevice();

    return state;
}

