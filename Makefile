

LIBS := -lX11

desktop-rectangle: desktop-rectangle.o
	gcc -o $@ $^ $(LIBS)

%.o: %.c
	gcc -c -o $@ $<

all: desktop-rectangle

clean:
	rm -rf desktop-rectangle *.o

.PHONY: all clean
