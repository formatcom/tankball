NAME=tankball

CC=g++

CFLAGS= -Wall -I. -lBox2D -lSDL2_gfx -lSDL2

SRCS= core/entity.cpp \
      core/game.cpp   \
      ground.cpp      \
      tank.cpp        \
      main.cpp


all: $(NAME)

$(NAME):
	$(CC) -o $@ $(SRCS) $(CFLAGS)

clean:
	$(RM) $(NAME)
