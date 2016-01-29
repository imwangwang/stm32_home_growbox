
# source files.
SRC =	stw_timer.c stw_system_timer.c

OBJ = $(SRC:.c=.o)

OUT = ../lib/stw_timer.a

# include directories
INCLUDES = -I. \
           -I../include/ \
           -I/usr/local/include

#          -I../../safe_base/include/ \

# C++ compiler flags (-g -O2 -Wall)
CCFLAGS = -g

# compiler
CCC = gcc

# library paths
LIBS = -L../ -L/usr/local/lib -lm

# compile flags
LDFLAGS = -g

.SUFFIXES: .c

default: dep $(OUT)

.c.o:
	$(CCC) $(INCLUDES) $(CCFLAGS) -c $< -o $@

$(OUT): $(OBJ)
	ar rcs $(OUT) $(OBJ)

depend: dep

dep:
	makedepend -- $(CFLAGS) -- $(INCLUDES) $(SRC)

clean:
	rm -f $(OBJ)  


# DO NOT DELETE

stw_timer.o: /usr/include/stdio.h /usr/include/_types.h
stw_timer.o: /usr/include/sys/_types.h /usr/include/sys/cdefs.h
stw_timer.o: /usr/include/machine/_types.h /usr/include/i386/_types.h
stw_timer.o: /usr/include/stdlib.h /usr/include/available.h
stw_timer.o: /usr/include/sys/wait.h /usr/include/sys/signal.h
stw_timer.o: /usr/include/sys/appleapiopts.h /usr/include/machine/signal.h
stw_timer.o: /usr/include/i386/signal.h /usr/include/i386/_structs.h
stw_timer.o: /usr/include/sys/_structs.h /usr/include/machine/_structs.h
stw_timer.o: /usr/include/sys/resource.h /usr/include/machine/endian.h
stw_timer.o: /usr/include/i386/endian.h /usr/include/sys/_endian.h
stw_timer.o: /usr/include/libkern/_OSByteOrder.h
stw_timer.o: /usr/include/libkern/i386/_OSByteOrder.h /usr/include/alloca.h
stw_timer.o: /usr/include/machine/types.h /usr/include/i386/types.h
stw_timer.o: /usr/include/string.h runtime_environment.h
stw_timer.o: ../include/safe_types.h ../include/stw_timer.h
stw_system_timer.o: ../include/stw_system_timer.h ../include/safe_types.h
stw_system_timer.o: ../include/stw_timer.h
