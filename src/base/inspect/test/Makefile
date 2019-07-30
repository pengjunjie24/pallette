
OBJ_DIR = ../../../obj
SOURCE_DIR = ../..

TARGET1 = InspectorTest

OBJ =$(wildcard $(OBJ_DIR)/*.o)

CC = g++
CFLAGS = -Wall -Wextra -Werror -std=c++11 -g
LIBS = -lmysqlcppconn -pthread -lrt -ldl -rdynamic
RM = rm

MAKE = make

.PHONY: all
all: subsystem $(TARGET1)
$(TARGET1): $(OBJ) 
	$(CC) $(OBJ) $(TARGET1).cpp $(LIBS) $(CFLAGS) -o $(TARGET1)

subsystem:
	cd $(SOURCE_DIR) && $(MAKE)

.PHONY: clean
clean:
	rm -rf $(TARGET1)
