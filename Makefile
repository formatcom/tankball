NAME=tankball

CC=g++

CFLAGS= -Wall -I. -lBox2D -lSDL2_gfx -lSDL2_ttf -lSDL2

SRCS= core/entity.cpp   \
      core/game.cpp     \
      core/contact.cpp  \
      controller.cpp    \
      ground.cpp        \
      tank.cpp          \
      logger.cpp        \
      bullet.cpp        \
      main.cpp


all: $(NAME)

$(NAME):
	$(CC) -o $@ $(SRCS) $(CFLAGS)

clean:
	$(RM) $(NAME)
