MAKEFLAGS += -s

NAME = ircserv

CC = c++ -g 
CCFLAGS = -Wall -Werror -Wextra -std=c++98
RM = rm
RMFLAGS= -f
GREEN=\033[0;32m
RED=\033[0;31m
YELLOW=\033[1;93m
NC=\033[0m

SRC = main.cpp \
	Ircserv.cpp\
	debugFuncions.cpp \
	helpFunctions.cpp \
	channelControl.cpp\
	utils.cpp \
	commands/pass.cpp \
	commands/ping.cpp \
	commands/join.cpp \
	commands/motd.cpp \
	commands/nick.cpp \
	commands/user.cpp \
	commands/privmsg.cpp \
	commands/quit.cpp \
	commands/mode.cpp \
	commands/kick.cpp \
	commands/invite.cpp \
	commands/unknown.cpp \
	signalHandler.cpp \
	bonus/bot.cpp \



OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CCFLAGS) $(OBJ) -o $(NAME)
	@echo "\n${YELLOW}--------------------------"
	@echo "${YELLOW}| ${GREEN}All files are compiled ${YELLOW}|"
	@echo "${YELLOW}--------------------------${NC}\n"

go:
	make re;
	./ircserv 6667 asd
	
govalgrind:
	make;
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --tool=memcheck ./ircserv 6667 123
clean: 
	$(RM) $(RMFLAGS) $(OBJ)
	$(RM) $(RMFLAGS) -r

	@echo "\n${YELLOW}-----------------------------"
	@echo "${YELLOW}| ${GREEN}Cleaned all ${RED}program${GREEN} files ${YELLOW}|"
	@echo "${YELLOW}-----------------------------${NC}\n"

fclean: clean
	$(RM) $(RMFLAGS) $(NAME)

	@echo "\n${YELLOW}--------------------------------"
	@echo "${YELLOW}| ${RED}ALL files ${GREEN}have been cleaned! ${YELLOW}|"
	@echo "${YELLOW}--------------------------------${NC}\n"

%.o: %.cpp
	$(CC) $(CCFLAGS) -o $@ -c $<

valgrind: re
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --tool=memcheck ./$(NAME)

re: fclean all