# Makefile for h264simpleCoder
# By Jordi Cenzano (www.jordicenzano.name)
CC=g++
CFLAGS=-c -Wall
HEADERS = CJOCh264bitstream.h CJOCh264encoder.h

all: h264simpleCoder

h264simpleCoder: h264simpleCoder.o CJOCh264encoder.o CJOCh264bitstream.o
	$(CC) h264simpleCoder.o CJOCh264encoder.o CJOCh264bitstream.o -o h264simpleCoder

h264simpleCoder.o: h264simpleCoder.cpp $(HEADERS)
	$(CC) $(CFLAGS) h264simpleCoder.cpp

CJOCh264encoder.o: CJOCh264encoder.cpp $(HEADERS)
	$(CC) $(CFLAGS) CJOCh264encoder.cpp

CJOCh264bitstream.o: CJOCh264bitstream.cpp $(HEADERS)
	$(CC) $(CFLAGS) CJOCh264bitstream.cpp

clean:
	rm -rf *o h264simpleCoder
