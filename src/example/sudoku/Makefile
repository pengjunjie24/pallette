
SOURCE_DIR  = ../../base
OBJ_DIR = ../../obj
SRC_DIR = .

TARGET = ServerBasic batch ServerThreadPool ServerMultiloop pipeline

TMP_OBJ =$(notdir $(wildcard $(SOURCE_DIR)/*.cpp))
OBJ = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(TMP_OBJ))

CC = g++
#INCLUDES = -I./include
CFLAGS = -Wall -Wextra -Werror -std=c++11 -g
LIBS = -pthread -lrt -ldl -rdynamic
RM = rm

MAKE = make

.PHONY: all
all: subsystem $(TARGET)
$(SRC_DIR)/%:  $(SRC_DIR)/%.cpp $(OBJ)
	$(CC) $(LIBS) $(CFLAGS) $(OBJ) Sudoku.cpp $< -o $@

subsystem:
	cd $(SOURCE_DIR) && $(MAKE)

.PHONY: clean
clean:
	rm -rf $(TARGET)
