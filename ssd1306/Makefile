SRC := $(wildcard *.c)
OBJ := $(SRC:.c=.o)
CFLAGS := -Wall -Wextra -Werror -O2 -std=c99 -I/usr/include/freetype2

libssd1306.so: $(OBJ)
	$(CC) $(CFLAGS) -shared -o $@ $^ -lwiringPi -lfreetype

libssd1306.a: $(OBJ)
	ar rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

clean:
	rm -f $(OBJ) libssd1306.so libssd1306.a
.PHONY: clean
