
CFLAGS=-O2 -Wall -gstabs+3 -I../include -I../examples/demo -D_WIN32
CXXFLAGS=-O2 -Wall -gstabs+3 -fno-exceptions -fno-rtti -I../include -I../examples/demo -D_WIN32

vpath %.cc ../examples/demo
vpath %.o ./obj_demo

DEMO_OBJS=./obj_demo/ascii.o ./obj_demo/calendar.o ./obj_demo/fileview.o\
	./obj_demo/gadgets.o ./obj_demo/help.o ./obj_demo/helpbase.o\
	./obj_demo/mousedlg.o ./obj_demo/puzzle.o ./obj_demo/tvdemo1.o\
	./obj_demo/tvdemo2.o ./obj_demo/tvdemo3.o

LIBS=-ltv -lstdc++ 
LDFLAGS=-L./

all:: demo.exe

obj_demo/%.o: %.cc
	$(COMPILE.cc) $< $(OUTPUT_OPTION)

demo.exe: $(DEMO_OBJS) libtv.a
	gcc $(LDFLAGS) -mconsole $(DEMO_OBJS) -o $@ $(LIBS)
