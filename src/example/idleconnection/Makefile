
SOURCE_DIR  = ../../base
OBJ_DIR = ../../obj

TARGET1 = EchoServer

TMP_OBJ =$(notdir $(wildcard $(SOURCE_DIR)/*.cpp))
OBJ = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(TMP_OBJ))

CC = g++
#INCLUDES = -I./include
CFLAGS = -Wall -Wextra -Werror -std=c++11 -g
LIBS = -pthread -lrt -ldl -rdynamic
RM = rm

MAKE = make

.PHONY: all
all: subsystem $(TARGET1)
$(TARGET1): $(OBJ) $(TARGET1).cpp $(TARGET1)Main.cpp
	$(CC) $(LIBS) $(CFLAGS) $(OBJ) $(TARGET1).cpp $(TARGET1)Main.cpp -o $(TARGET1)

subsystem:
	cd $(SOURCE_DIR) && $(MAKE)

.PHONY: clean
clean:
	rm -rf $(TARGET1) $(TARGET2)
