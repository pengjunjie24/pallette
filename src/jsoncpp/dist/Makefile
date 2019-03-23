SRC_DIR = .
OBJ_DIR = ../../obj

TMP_SRC = $(wildcard $(SRC_DIR)/*.cpp)
SRC = $(filter-out $(filter %_$(second).cpp, $(TMP_SRC)), $(TMP_SRC))

TMP_OBJ = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir $(SRC)))
OBJ = $(filter-out $(filter %_$(second).o, $(TMP_OBJ)), $(TMP_OBJ))

CC = g++
#INCLUDES = -I./include
CFLAGS = -Wall -Wextra -Werror -std=c++11 -g
RM = rm
MAKE = make

.PHONY: all
all: $(OBJ) 
$(OBJ_DIR)/%.o:$(SRC_DIR)/%.cpp
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: print
print:
	@echo $(SRC); echo $(OBJ)

.PHONY: clean
clean:
	rm -rf $(OBJ)
