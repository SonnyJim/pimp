CC	= gcc
SDL2_CFLAGS := $(shell sdl2-config --cflags)
SDL2_LDFLAGS := $(shell sdl2-config --libs)
CFLAGS = -O3 -Wall -Winline -pipe -std=gnu99 $(SDL2_CFLAGS) -I./src/include
LDLIBS = $(SDL2_LDFLAGS)

OBJS = pimp.c

pimp: $(OBJS)
	@echo [link]
	@$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

clean:
	@echo "[Removing object files]"
	@find . -name '*.o' -delete
	@echo "[Removing binaries]"
	@rm -f pimp
