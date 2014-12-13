SRC_DIR := ./src
TEST_SRC_DIR := ./t
APP_SRC_DIR := ./app

INCLUDE_DIR := ./include
BUILD_DIR := ./build
TESTBIN_DIR := $(BUILD_DIR)/test
APPBIN_DIR := ./bin

CPP := g++
CC := gcc
AR := ar

CFLAG := -g -Wall -std=c++11
LFLAG := -g -lpthread
ARFLAG := -rcs

SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(patsubst %.cpp,%.o, $(subst $(SRC_DIR),$(BUILD_DIR), $(SRC)))
APP_SRC := $(wildcard $(APP_SRC_DIR)/*.cpp)
APP_TARGET := $(patsubst %.cpp, %, $(subst $(APP_SRC_DIR), $(APPBIN_DIR), $(APP_SRC)))
TEST_SRC := $(wildcard $(TEST_SRC_DIR)/*.cpp)
TEST_TARGET := $(patsubst %.cpp, %, $(subst $(TEST_SRC_DIR),$(TESTBIN_DIR), $(TEST_SRC)))

ARCHIVE := libtwpc.a

.PHONY:all target test $(BUILD_DIR) $(TESTBIN_DIR) $(APPBIN_DIR)
all: test target
test: $(TESTBIN_DIR) $(TEST_TARGET) target
target: $(BUILD_DIR) $(OBJ) $(ARCHIVE) $(APPBIN_DIR) $(APP_TARGET)

$(BUILD_DIR):
	mkdir -p $@

$(APPBIN_DIR):
	mkdir -p $@

$(BUILD_DIR)/%.o:$(SRC_DIR)/%.cpp
	$(CPP) -c $< $(CFLAG) -o $@ -I$(INCLUDE_DIR)

$(ARCHIVE):$(OBJ)
	$(AR) $(ARFLAG) $@ $(OBJ)

$(TESTBIN_DIR):
	mkdir -p $@


$(TESTBIN_DIR)/%:$(TEST_SRC_DIR)/%.cpp $(OBJ)
	$(CPP) $^ $(CFLAG) $(LFLAG) -o $@ -I$(INCLUDE_DIR) -L$(BUILD_DIR)

$(APPBIN_DIR)/%:$(APP_SRC_DIR)/%.cpp $(ARCHIVE)
	$(CPP) $^ $(CFLAG) $(LFLAG) -o $@ -I$(INCLUDE_DIR) -L$(BUILD_DIR) -L./

clean:
	rm -rf $(BUILD_DIR) $(TESTBIN_DIR) $(ARCHIVE) $(APPBIN_DIR)
