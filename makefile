CC = gcc
CFLAGS = -Wall -Wdeclaration-after-statement -Wno-unused -Wextra #-ansi #-pedantic 
LDLIBS = -lGL -lGLU -lglut

UNAME = $(shell uname)
ifeq ($(UNAME), Darwin)  # Mac OS X
LDLIBS = -framework OpenGL -framework GLUT
endif

BINS = drawSplines

all: $(BINS)

drawSplines: evalSpline.o

clean:
	@rm -f *.o *~ $(BINS)
