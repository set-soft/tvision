# Makefile for gcc-2.95.2 (mingw32)
CFLAGS=-O2 -Wall -Werror -gstabs+3
CXXFLAGS=-O2 -Wall -Werror -gstabs+3 -fno-exceptions -fno-rtti
CC=gcc

vpath %.cc ./ ../examples/tutorial ../examples/demo
vpath %.o  ./obj_demo

DEMO_OBJS=./obj_demo/ascii.o ./obj_demo/calendar.o ./obj_demo/fileview.o\
	./obj_demo/gadgets.o ./obj_demo/help.o ./obj_demo/helpbase.o\
	./obj_demo/mousedlg.o ./obj_demo/puzzle.o ./obj_demo/tvdemo1.o\
	./obj_demo/tvdemo2.o ./obj_demo/tvdemo3.o

LIBS=-lrhtv -lstdc++
LDFLAGS=-L./

obj_demo/%.o: %.cc
	$(COMPILE.cc) -I../include -I../examples/demo $< $(OUTPUT_OPTION)

all:: demo.exe

demo.exe: $(DEMO_OBJS) librhtv.a
	gcc $(LDFLAGS) -mconsole $(DEMO_OBJS) -o $@ $(LIBS)
   
clean:
	rm -f ./obj_demo/*.o

