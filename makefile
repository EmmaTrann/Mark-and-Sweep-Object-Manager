CC = clang
CFLAGS = -Wall
OBJECTS = main.o ObjectManager.o

all: main
main: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o main
main.o: main.c ObjectManager.h
	$(CC) $(CFLAGS) -c main.c
ObjectManager.o: ObjectManager.c ObjectManager.h
	$(CC) $(CFLAGS) -c ObjectManager.c
clean:
	rm -f $(OBJECTS) main
