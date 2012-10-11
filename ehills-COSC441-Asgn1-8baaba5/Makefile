CC = gcc
CFLAGS = -c -Wall 
LDFLAGS = -pthread
SOURCES = master.c worker.c reader.c writer.c disc.c mylib.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = simulation

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

clean:
	rm -rf *.o $(EXECUTABLE)
