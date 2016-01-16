########################################################################### 
# Compiler
########################################################################### 

CC = gcc
MPICC = mpicc
CFLAGS = -std=c99
INCFLAGS = -I$(HOME)/usr/include -lpopt
LDFLAGS = -L$(HOME)/usr/lib -lpopt -fopenmp

ifeq ($(DEBUG),on)
	CFLAGS += -DDEBUG
endif

########################################################################### 
# File Declarations
########################################################################### 
BINARIES = go-Serial go-MPI

# C, H, and Object Files
C_FILES = go_Serial.c board_tools.c influence.c rules.c score.c serial_move_tree.c
H_FILES = go_definitions.h board_tools.h rules.h board_tools.h score.h serial_move_tree.h
O_FILES = go_Serial.o board_tools.o influence.o rules.o score.o serial_move_tree.o

MPI_C_FILES = go_MPI.c board_tools.c influence.c rules.c score.c parallel_move_tree.c
MPI_H_FILES = go_definitions.h board_tools.h rules.h board_tools.h score.h parallel_move_tree.h
MPI_O_FILES = go_MPI.o board_tools.o influence.o rules.o score.o parallel_move_tree.o

########################################################################### 
# Main Targets
########################################################################### 
all: $(BINARIES)

janus: $(BINARIES)

go-Serial: $(O_FILES)
	$(CC) $(INCFLAGS) -o go-Serial $(O_FILES) $(LDFLAGS)
	
go-MPI: $(MPI_O_FILES)
	$(MPICC) $(INCFLAGS) -o go-MPI $(MPI_O_FILES) $(LDFLAGS)

clean:
	/bin/rm -rf core $(O_FILES) $(MPI_O_FILES) $(BINARIES)

# Suffixes

.c.o:		
	$(MPICC) $(CFLAGS) $(INCFLAGS) -c -o $*.o $*.c $(LDFLAGS)

# Dependencies
