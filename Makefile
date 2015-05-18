OBJS = USBswitch.o \
	   USBaccessBasic.o \
	   USBaccess.o \
	   USBaccess.obj

LIBS = USBaccess.a

all: USBswitch

USBswitch: $(OBJS) $(LIBS)
	g++ USBswitch.o USBaccess.a -o USBswitch

USBaccess.a: USBaccessBasic.o USBaccess.o
	ld USBaccessBasic.o USBaccess.o -Ur -o USBaccess.obj
	ar rc USBaccess.a USBaccess.obj

USBaccessBasic.o: USBaccessBasic.c
	cc -c USBaccessBasic.c

USBaccess.o: USBaccess.cpp USBaccessBasic.o
	g++ -c USBaccess.cpp

USBswitch.o: USBswitch.cpp USBaccess.a
	g++ -c USBswitch.cpp

clean:
	rm -f $(OBJS) $(LIBS) USBswitch
