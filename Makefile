USBACCESSFILES = USBaccessBasic.o USBaccess.o

USBaccess.a: $(USBACCESSFILES)
	ld $(USBACCESSFILES) -Ur -o USBaccess.obj
	ar rc USBaccess.a USBaccess.obj

USBswitch: USBswitch.o USBaccess.a
		g++ USBswitch.o USBaccess.a -o USBswitch

all: USBswitch 

clean:
	rm -f *.o *.a *.obj USBswitch
