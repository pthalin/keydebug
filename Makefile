# includes and libs
INCS = -I. 
LIBS = -lc -lutil

# flags
CPPFLAGS = -Wno-write-strings
CFLAGS  += -g -Wall ${INCS} ${CPPFLAGS}
LDFLAGS += -g ${LIBS} -lSDL -lSDL_image -lSDL_ttf

# compiler and linker
CC ?= cc
CXX = g++

SRC = keydebug.c
OBJ = ${SRC:.c=.o}
all: options keydebug 

options:
	@echo kbd build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CXX       = ${CXX}"

.c.o:
	@echo CXX $<
	@${CXX} -c ${CFLAGS} $<



keydebug: 	${OBJ}
	@echo CXX -o $@
	@${CXX} -o $@ ${OBJ} ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f keydebug ${OBJ} 
