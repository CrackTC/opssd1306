SRC_EXAMPLE := $(wildcard example/*.c)
OBJ_EXAMPLE := $(SRC_EXAMPLE:.c=.o)
CFLAGS := -Wall -Wextra -Werror -O2 -std=c99 -Issd1306

example/example: $(OBJ_EXAMPLE) ssd1306/libopssd1306.a
	$(CC) $(CFLAGS) -o $@ $^ ssd1306/libopssd1306.a -lwiringPi -lfreetype

ssd1306/libopssd1306.a:
	$(MAKE) -C ssd1306 libopssd1306.a
.PHONY: ssd1306/libopssd1306.a

ssd1306/libopssd1306.so:
	$(MAKE) -C ssd1306 libopssd1306.so

-include $(SRC_EXAMPLE:.c=.d)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.d: %.c
	$(CC) $(CFLAGS) -MM -MT $(@:.d=.o) -o $@ $<

run: example/example
	./example/example
.PHONY: run

OpSsd1306.lib: ssd1306/libopssd1306.so
	$(MAKE) -C bindings/cs/lib pack
.PHONY: OpSsd1306.lib

OpSsd1306: OpSsd1306.lib
	$(MAKE) -C bindings/cs/OpSsd1306 pack
.PHONY: OpSsd1306

clean:
	$(MAKE) -C ssd1306 clean
	$(MAKE) -C bindings/cs/lib clean
	$(MAKE) -C bindings/cs/OpSsd1306 clean
	rm -f $(OBJ_EXAMPLE) $(SRC_EXAMPLE:.c=.d) example/example
.PHONY: clean
