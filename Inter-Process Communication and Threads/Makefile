###############################################
# Makefile for compiling the program skeleton
# 'make' build executable file 'PROJ'
# 'make doxy' build project manual in doxygen
# 'make all' build project + manual
# 'make clean' removes all .o, executable and doxy log
###############################################
PROJ = FindTheLastBall # the name of the project
CC = gcc # name of compiler
DOXYGEN = doxygen # name of doxygen binary
# define any compile-time flags
CFLAGS = -Wall -O -Wuninitialized -Wunreachable-code # there is a space at the end of this
LFLAGS = -pthread 
###############################################
# You don't need to edit anything below this line
###############################################
# list of object files
# The following includes all of them!
C_FILES := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(C_FILES))
# To create the executable file we need the individual
# object files
make:
	gcc server.c -o server -Wall
	gcc -pthread client.c -o client -Wall
	#make doxy
# To create each individual object file we need to
# compile these files using the following general
# purpose macro 
# there is a TAB for each identation.
# To make all (program + manual) "make all"
all:
	make
	make doxy
# To make all (program + manual) "make doxy"
doxy:
	$(DOXYGEN) doxygen.conf &> doxygen.log
# To clean .o files: "make clean"
clean:
	rm -rf *.o doxygen.log html
