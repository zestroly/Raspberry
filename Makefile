LIBTARDIR=lib

BASEDIR=$(PWD)
CC=arm-linux-gnueabihf-gcc --sysroot=/opt/Raspberry/rootfs
CXX=arm-linux-gnueabihf-g++  --sysroot=/opt/Raspberry/rootfs


INCLUDE= -Iinclude
LFLAGS= -shared -Wl,-soname,libRaspDriver.so
CFLAGS= -pipe -g -Wall -W -fPIC
CXXFLAGS=-pipe -fPIC -std=gnu++11
LIBS    =  -pthread -Llib -lwiringPi 

TARGET0  =$(LIBTARDIR)/libRaspDriver.so
SRCDIR  = src
OBJECTS = $(SRCDIR)/RaspberryCapture.o \
		  $(SRCDIR)/RaspberryOverlay.o \
		  $(SRCDIR)/RaspberryIO.o \
		  $(SRCDIR)/RaspberryI2CBus.o \
		  $(SRCDIR)/RaspberryLog.o \
		  $(SRCDIR)/RaspberryDriver.o

all: $(TARGET0) test install

$(TARGET0):$(OBJECTS)
	$(CXX) $(LFLAGS) $(CXXFLAGS) $(OBJECTS) -o $(TARGET0) $(LIBS)
	cp src/*.h include
	cp lib/* bin/

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) -o "$@" "$<"

.c.o:
	$(CC)  -c -o "$@" "$<" 

.PHONY:test

testobj = testRaspberrySensor  testoverlay testi2c

testobjtemp := $(testobj:%=bin/%)
test:$(testobjtemp)

TESTCXXFLAGS = -Iinclude -Llib -std=gnu++11 -Wl,--rpath=./ -lRaspDriver  -lwiringPi -llog4cpp


bin/testRaspberrySensor:test/testRaspSensor.o test/rgbtobmp.o
	$(CXX) $^ -o $@ $(TESTCXXFLAGS) 

bin/testoverlay:test/testoverlay.o
	$(CXX) $^ -o $@ $(TESTCXXFLAGS)

bin/testi2c:test/testi2c.o
	$(CXX) $^ -o $@ $(TESTCXXFLAGS)

clean:
	rm $(testobj:%=test/%.o) -rf
	rm $(testobjtemp) -rf
	rm $(OBJECTS) -rf
	rm $(TARGET0) -rf
	rm include/Raspberry* -rf
bsp:
	tar zcvf libdriver.tar.bz include lib doc

install:
	cp script/* bin
