SRC_EXAMPLE := $(wildcard example/*.c)
OBJ_EXAMPLE := $(SRC_EXAMPLE:.c=.o)
CFLAGS := -Wall -Wextra -Werror -O2 -std=c99 -Issd1306

example/example: $(OBJ_EXAMPLE) ssd1306/libssd1306.a
	$(CC) $(CFLAGS) -o $@ $^ ssd1306/libssd1306.a -lwiringPi -lfreetype

ssd1306/libssd1306.a:
	$(MAKE) -C ssd1306 libssd1306.a
.PHONY: ssd1306/libssd1306.a

-include $(SRC_EXAMPLE:.c=.d)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.d: %.c
	$(CC) $(CFLAGS) -MM -MT $(@:.d=.o) -o $@ $<

run: example/example
	./example/example

clean:
	$(MAKE) -C ssd1306 clean
	rm -f $(OBJ_EXAMPLE) $(SRC_EXAMPLE:.c=.d) example/example
.PHONY: clean
