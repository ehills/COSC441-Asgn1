CC = gcc
CFLAGS = -c -Wall -o3
LDFLAGS =
SOURCES = master.c worker.c reader.c writer.c disc.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = simulation

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

clean:
	rm -rf *o $(EXECUTABLE)
