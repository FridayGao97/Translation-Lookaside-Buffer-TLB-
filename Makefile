  # the compiler: gcc for C program, define as g++ for C++
CC = gcc

  # compiler flags:
  #  -g    adds debugging information to the executable file
  #  -Wall turns on most, but not all, compiler warnings
CFLAGS  =  -std=c99 -Wall
OBJFILES = heapsort mergesort quicksort
  # the build target executable:
TARGET = valtlb379

all: $(TARGET) $(OBJFILES) 
heapsort: heapsort.c
	           $(CC) $(CFLAGS) -o heapsort heapsort.c

mergesort: mergesort.c
	           $(CC) $(CFLAGS) -o mergesort mergesort.c

quicksort: quicksort.c
	           $(CC) $(CFLAGS) -o quicksort quicksort.c

$(TARGET): $(TARGET).c
	   $(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) $(TARGET)
	$(RM) $(OBJFILES)