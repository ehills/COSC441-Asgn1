CC = gcc -g -pg
CFLAGS = -c -Wall
LDFLAGS = -pthread -g -pg
SOURCES = master.c worker.c reader.c writer.c disc.c mylib.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = simulation

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ -pg

clean:
	rm -rf *.o $(EXECUTABLE)
