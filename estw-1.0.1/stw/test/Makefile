#
#
#
#include paths 
INCLUDE = -I. -I../include -I/usr/local/include

#project sources 
SRC =   demo_stw.c


#project objects 
OBJ =   demo_stw.o

LIB =   ../lib/stw_timer.a 


IMAGE =	demo_stw  


#compiler flags 
CCC = gcc  
DEBUG = -g
CFLAGS =  -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)



$(IMAGE): dep $(OBJ)
	$(CCC) $(INCLUDE) $(LFLAGS) $(OBJ) $(LIB) -o $(IMAGE)

depend:	dep

dep:
	makedepend -- $(CFLAGS) -- $(INCLUDE) $(SRC)

clean:
	rm -f $(OBJ) 



# DO NOT DELETE 

demo_stw.o: /usr/include/stdio.h /usr/include/_types.h
demo_stw.o: /usr/include/sys/_types.h /usr/include/sys/cdefs.h
demo_stw.o: /usr/include/machine/_types.h /usr/include/i386/_types.h
demo_stw.o: /usr/include/string.h /usr/include/stdlib.h
demo_stw.o: /usr/include/available.h /usr/include/sys/wait.h
demo_stw.o: /usr/include/sys/signal.h /usr/include/sys/appleapiopts.h
demo_stw.o: /usr/include/machine/signal.h /usr/include/i386/signal.h
demo_stw.o: /usr/include/i386/_structs.h /usr/include/sys/_structs.h
demo_stw.o: /usr/include/machine/_structs.h /usr/include/sys/resource.h
demo_stw.o: /usr/include/machine/endian.h /usr/include/i386/endian.h
demo_stw.o: /usr/include/sys/_endian.h /usr/include/libkern/_OSByteOrder.h
demo_stw.o: /usr/include/libkern/i386/_OSByteOrder.h /usr/include/alloca.h
demo_stw.o: /usr/include/machine/types.h /usr/include/i386/types.h
demo_stw.o: /usr/include/inttypes.h /usr/include/stdint.h
demo_stw.o: /usr/include/signal.h /usr/include/sys/time.h /usr/include/time.h
demo_stw.o: /usr/include/_structs.h /usr/include/sys/_select.h
demo_stw.o: /usr/include/sys/types.h /usr/include/unistd.h
demo_stw.o: /usr/include/sys/unistd.h /usr/include/sys/select.h
demo_stw.o: ../include/safe_types.h ../include/stw_system_timer.h
demo_stw.o: ../include/safe_types.h ../include/stw_timer.h
demo_stw.o: ../include/stw_system_timer.h
