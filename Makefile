LIBTARDIR=lib

BASEDIR=$(PWD)


INCLUDE= -Iinclude
LFLAGS= -shared -Wl,-soname,libRaspDriver.so
CFLAGS= -pipe -g -Wall -W -fPIC
CXXFLAGS=-pipe -fPIC -std=gnu++11
LIBS    =  -pthread

TARGET0  =$(LIBTARDIR)/libRaspDriver.so
SRCDIR  = src
OBJECTS = $(SRCDIR)/RaspberryCapture.o \
		  $(SRCDIR)/RaspberryIO.o \
		  $(SRCDIR)/RaspberryCamera.o \
		  $(SRCDIR)/libcapture.o 

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

testobj = testRaspberrySensor t_libcapture

testobjtemp := $(testobj:%=bin/%)
test:$(testobjtemp)

TESTCXXFLAGS = -Iinclude -Llib -std=gnu++11 -Wl,--rpath=./ -lRaspDriver

bin/testRaspberrySensor:test/testRaspSensor.o
	$(CXX) $^ -o $@ $(TESTCXXFLAGS) 

bin/t_libcapture:test/t_libcapture.o
	$(CXX) $^ -o $@ $(TESTCXXFLAGS) 


clean:
	rm $(testobj:%=test/%.o) -rf
	rm $(testobjtemp) -rf
	rm $(OBJECTS) -rf
	rm $(TARGET0) -rf
	rm include/* -rf
bsp:
	tar zcvf libdriver.tar.bz include lib doc

install:
	cp script/* bin
