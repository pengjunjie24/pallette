
SOURCE_DIR  = ..
OBJ_DIR = ../../../obj

TARGET1 = MysqlConnectionPoolTest

TMP_OBJ =$(notdir $(wildcard $(SOURCE_DIR)/*.cpp))
OBJ = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(TMP_OBJ))

CC = g++
CFLAGS = -Wall -Wextra -Werror -std=c++11 -g
LIBS = -lmysqlcppconn -pthread -lrt -ldl -rdynamic
RM = rm

MAKE = make

.PHONY: all
all: subsystem $(TARGET1)
$(TARGET1): $(OBJ) $(TARGET1).cpp
	$(CC) $(OBJ) $(TARGET1).cpp $(LIBS) $(CFLAGS) -o $(TARGET1)

subsystem:
	cd $(SOURCE_DIR) && $(MAKE)

.PHONY: clean
clean:
	rm -rf $(TARGET1)
