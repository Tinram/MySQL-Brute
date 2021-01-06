
# makefile for MySQL Brute

CC = gcc
NAME = mysqlbrute

CFLAGS = -Ofast -Wall -Wextra -Wuninitialized -Wunused -Werror -Wformat=2 -Wunused-parameter -Wshadow -Wwrite-strings -Wstrict-prototypes -Wold-style-definition -Wredundant-decls -Wnested-externs -Wmissing-include-dirs -Wformat-security -std=gnu99 -flto -s

MYSQLCFLAGS = $(shell mysql_config --cflags)
MYSQLLIBS = $(shell mysql_config --libs)

$(NAME):
	$(CC) $(NAME).c -o $(NAME) $(CFLAGS) $(MYSQLCFLAGS) $(MYSQLLIBS)

install:
	sudo cp $(NAME) /usr/local/bin/$(NAME)
	@echo "Attempted to copy $(NAME) to /usr/local/bin"

deps:
	sudo apt install libmysqlclient-dev
	sudo apt install libssl-dev
