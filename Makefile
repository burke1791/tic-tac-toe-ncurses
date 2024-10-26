CC = gcc
CFLAGS = -I./include -lncursesw -fsanitize=address -fsanitize=undefined -static-libasan -g

TARGET_EXEC = ttt

SRC_FILES = main.c \
						src/board.c \
						src/display.c \
						src/game.c

$(TARGET_EXEC): ${SRC_FILES}
	${CC} ${CFLAGS} -o $@ $?

clean:
	rm -f $(TARGET_EXEC)

# recover terminal: stty sane ^J