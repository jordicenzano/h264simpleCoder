# Makefile for h264simpleCoder
# By Jordi Cenzano (www.jordicenzano.name)
CC=g++
CFLAGS=-c -Wall
HEADERS = CJOCbitstream.h CJOCh264encoder.h

all: h264simpleCoder

h264simpleCoder: h264simpleCoder.o CJOCh264encoder.o CJOCbitstream.o
	$(CC) h264simpleCoder.o CJOCh264encoder.o CJOCbitstream.o -o h264simpleCoder

h264simpleCoder.o: h264simpleCoder.cpp $(HEADERS)
	$(CC) $(CFLAGS) h264simpleCoder.cpp

CJOCh264encoder.o: CJOCh264encoder.cpp $(HEADERS)
	$(CC) $(CFLAGS) CJOCh264encoder.cpp

CJOCbitstream.o: CJOCbitstream.cpp $(HEADERS)
	$(CC) $(CFLAGS) CJOCbitstream.cpp

clean:
	rm -rf *o h264simpleCoder
