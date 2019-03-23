SRC_DIR = .
OBJ_DIR = ../../obj
BIN_DIR = bin
SUBDIR = ../dist

SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(wildcard $(OBJ_DIR)/*.o)
TARGET = $(patsubst %.cpp, $(BIN_DIR)/%, $(notdir $(SRC)))

CC = g++
#INCLUDES = -I./include
CFLAGS = -Wall -Wextra -Werror -std=c++11 -DNDEBUG -g
LIBS = -pthread -lrt -ldl -rdynamic
RM = rm
MAKE = make

.PHONY: all
all: subsystem $(TARGET)
$(BIN_DIR)/%:$(SRC_DIR)/%.cpp $(OBJ)
	$(CC) $(LIBS) $(CFLAGS) $(OBJ) $< -o $@

subsystem:
	cd $(SUBDIR) && $(MAKE)

.PHONY: print
print:
	@echo $(SRC); echo $(OBJ); echo $(TARGET)

.PHONY: clean
clean:
	rm -rf $(TARGET)
