SRC_DIR = .
OBJ_DIR = ../../obj

SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir $(SRC)))

CC = g++
CFLAGS = -Wall -Wextra -Werror -std=c++11 -g
LIBS = -pthread -lrt -ldl -rdynamic
RM = rm
MAKE = make

.PHONY: all
all: $(OBJ)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: print
print:
	@echo $(OBJ); echo $(SRC); echo $(OBJ_DIR)

.PHONY: clean
clean:
	rm -rf $(OBJ)
