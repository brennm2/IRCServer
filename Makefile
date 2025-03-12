MAKEFLAGS += -s

NAME = ircserv
NAME_BONUS = ircserv_bonus

CC = c++ -g
CCFLAGS = -Wall -Werror -Wextra -std=c++98
RM = rm
RMFLAGS= -f
GREEN=\033[0;32m
RED=\033[0;31m
YELLOW=\033[1;93m
NC=\033[0m

SRC = base/main.cpp \
	base/Ircserv.cpp\
	base/debugFuncions.cpp \
	base/helpFunctions.cpp \
	base/commands/pass.cpp \
	base/commands/ping.cpp \
	base/commands/join.cpp \
	base/commands/motd.cpp \
	base/commands/nick.cpp \
	base/commands/user.cpp \
	base/commands/privmsg.cpp \
	base/commands/quit.cpp \
	base/commands/mode.cpp \
	base/commands/kick.cpp \
	base/commands/invite.cpp \
	base/commands/unknown.cpp \
	base/commands/part.cpp \
	base/commands/topic.cpp \
	base/signalHandler.cpp \

OBJ = $(SRC:.cpp=.o)

SRCBONUS = bonus/main.cpp \
	bonus/Ircserv.cpp\
	bonus/debugFuncions.cpp \
	bonus/helpFunctions.cpp \
	bonus/commands/pass.cpp \
	bonus/commands/ping.cpp \
	bonus/commands/join.cpp \
	bonus/commands/motd.cpp \
	bonus/commands/nick.cpp \
	bonus/commands/user.cpp \
	bonus/commands/privmsg.cpp \
	bonus/commands/quit.cpp \
	bonus/commands/mode.cpp \
	bonus/commands/kick.cpp \
	bonus/commands/invite.cpp \
	bonus/commands/unknown.cpp \
	bonus/commands/part.cpp \
	bonus/commands/topic.cpp \
	bonus/signalHandler.cpp \
	bonus/bonus/bot.cpp \

OBJBONUS = $(SRCBONUS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CCFLAGS) $(OBJ) -o $(NAME)
	@echo "\n${YELLOW}--------------------------"
	@echo "${YELLOW}| ${GREEN}All files are compiled ${YELLOW}|"
	@echo "${YELLOW}--------------------------${NC}\n"

bonus: $(NAME_BONUS)

$(NAME_BONUS): $(OBJBONUS)
	$(CC) $(CCFLAGS) $(OBJBONUS) -o $(NAME_BONUS)
	@echo "\n${YELLOW}--------------------------"
	@echo "${YELLOW}| ${GREEN}All files are compiled ${YELLOW}|"
	@echo "${YELLOW}--------------------------${NC}\n"
	
govalgrind:
	make;
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --tool=memcheck ./ircserv 6667 123

govalgrindbonus:
	make bonus;
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --tool=memcheck ./ircserv_bonus 6667 123
clean: 
	$(RM) $(RMFLAGS) $(OBJ)
	$(RM) $(RMFLAGS) -r
	$(RM) $(RMFLAGS) $(NAME_BONUS)

	@echo "\n${YELLOW}-----------------------------"
	@echo "${YELLOW}| ${GREEN}Cleaned all ${RED}program${GREEN} files ${YELLOW}|"
	@echo "${YELLOW}-----------------------------${NC}\n"

fclean: clean
	$(RM) $(RMFLAGS) $(NAME)
	$(RM) $(RMFLAGS) $(OBJBONUS)

	@echo "\n${YELLOW}--------------------------------"
	@echo "${YELLOW}| ${RED}ALL files ${GREEN}have been cleaned! ${YELLOW}|"
	@echo "${YELLOW}--------------------------------${NC}\n"

%.o: %.cpp
	$(CC) $(CCFLAGS) -o $@ -c $<

valgrind: re
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --tool=memcheck ./$(NAME)

re: fclean all