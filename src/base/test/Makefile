SRC_DIR = .
BIN_DIR = bin
OBJ_DIR = ../../obj
SOURCE_DIR = ..
SUBDIR = ..

SRC = $(wildcard $(SRC_DIR)/*.cpp)
TMP_OBJ =$(notdir $(wildcard $(SOURCE_DIR)/*.cpp))
OBJ = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(TMP_OBJ))
TARGET = $(patsubst %.cpp, $(BIN_DIR)/%, $(notdir $(SRC)))

CC = g++
#INCLUDES = -I./include
CFLAGS = -Wall -Wextra -Werror -std=c++11 -g
LIBS = -pthread -lrt -ldl -rdynamic
RM = rm

MAKE = make

.PHONY: all
all: subsystem $(TARGET)
$(BIN_DIR)/%: $(SRC_DIR)/%.cpp $(OBJ)
	$(CC) $(LIBS) $(CFLAGS) $(OBJ) $< -o $@

subsystem:
	cd $(SUBDIR) && $(MAKE)

.PHONY: print
print:
	@echo $(SRC); echo $(OBJ); echo $(TARGET)

.PHONY: clean
clean:
	rm -rf $(TARGET)
