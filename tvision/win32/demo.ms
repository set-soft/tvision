
CFLAGS=-O2 -D_WIN32 /GR- /GX-
CXXFLAGS=-O2 -I../include -I../examples/demo -D_WIN32 /GR- /GX-

vpath %.cc ../examples/demo
vpath %.o ./obj_demo_ms

CC=cl /nologo
CXX=cl /nologo /TP

DEMO_OBJS=./obj_demo_ms/ascii.o ./obj_demo_ms/calendar.o ./obj_demo_ms/fileview.o\
	./obj_demo_ms/gadgets.o ./obj_demo_ms/help.o ./obj_demo_ms/helpbase.o\
	./obj_demo_ms/mousedlg.o ./obj_demo_ms/puzzle.o ./obj_demo_ms/tvdemo1.o\
	./obj_demo_ms/tvdemo2.o ./obj_demo_ms/tvdemo3.o

LIBS=libtv.lib

all:: demo.exe

obj_demo_ms/%.o: %.cc
	$(COMPILE.cc) $(subst /,\\,$<) -Fo$(subst /,\\,$@)

demo.exe: $(DEMO_OBJS) libtv.lib
	link $(LDFLAGS) $(subst /,\\,$(DEMO_OBJS)) /OUT:$@ $(LIBS)
