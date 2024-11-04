SRC := $(wildcard *.c)
OBJ := $(SRC:.c=.o)
CFLAGS := -Wall -Wextra -Werror -O2 -std=c99 -I/usr/include -L/usr/lib -L/usr/lib/aarch64-linux-gnu -I/usr/include/freetype2

libopssd1306.so: $(OBJ)
	$(CC) $(CFLAGS) -shared -o $@ $^ -lwiringPi -lfreetype

libopssd1306.a: $(OBJ)
	ar rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

clean:
	rm -f $(OBJ) libopssd1306.so libopssd1306.a
.PHONY: clean
