PROG = ykush
CC = g++
CPPFLAGS = -Wall
LDFLAGS = -lusb-1.0
OBJS = main.o usbcom.o

$(PROG) : $(OBJS)
	$(CC) -o $(PROG) $(OBJS) $(LDFLAGS)
	
main.o : main.cpp
	$(CC) $(CPPFLAGS) -c main.cpp
	
usbcom.o : usbcom.cpp usbcom.h
	$(CC) $(CPPFLAGS) -c usbcom.cpp

clean : 
	rm -f $(PROG) $(OBJS)