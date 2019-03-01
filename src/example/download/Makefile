
SOURCE_DIR  = ../../base
OBJ_DIR = ../../obj

TARGET1 = DownloadServer
TARGET2 = DownloadClient

TMP_OBJ =$(notdir $(wildcard $(SOURCE_DIR)/*.cpp))
OBJ = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(TMP_OBJ))

CC = g++
#INCLUDES = -I./include
CFLAGS = -Wall -Wextra -Werror -std=c++11 -g
LIBS = -pthread -lrt -ldl -rdynamic
RM = rm

MAKE = make

.PHONY: all
all: subsystem $(TARGET1) $(TARGET2)
$(TARGET1): $(OBJ)
	$(CC) $(LIBS) $(CFLAGS) $(OBJ) $(TARGET1).cpp $(TARGET1)MAIN.cpp -o $(TARGET1)

$(TARGET2): $(OBJ)
	$(CC) $(LIBS) $(CFLAGS) $(OBJ) $(TARGET2).cpp $(TARGET2)MAIN.cpp -o $(TARGET2)

subsystem:
	cd $(SOURCE_DIR) && $(MAKE)

.PHONY: clean
clean:
	rm -rf $(TARGET1) $(TARGET2)
