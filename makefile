CC			= gcc
CFLAGS		= -ansi -pedantic -Wall 
PROG_NAME 	= assembler
BUILD_DIR	= build
OBJ_DIR		= $(BUILD_DIR)/obj
BIN_DIR		= $(BUILD_DIR)/bin
 
.PHONY: clean build_env all
 
all: build_env $(PROG_NAME) 
 
$(PROG_NAME): linkedlist.o macro_handler.o preasm.o tools.o errors.o lexer.o assembler.o firstpass.o dynamic_arr.o utils_first_pass.o test.o
	$(CC) $(CFLAGS) $(OBJ_DIR)/*.o -o ./$@ 
 
assembler.o: assembler.c lexer/lexer.h lexer/../preasm/macro_handler.h \
 lexer/../preasm/../data_stractures/linkedlist.h \
 lexer/../preasm/../utils/tools.h \
 lexer/../preasm/../utils/../utils/errors.h preasm/preasm.h \
 preasm/../utils/tools.h preasm/../data_stractures/linkedlist.h \
 preasm/../preasm/macro_handler.h passes/firstpass.h \
 passes/utils_first_pass.h passes/../data_stractures/dynamic_arr.h \
 passes/../data_stractures/linkedlist.h passes/../utils/tools.h \
 passes/../lexer/lexer.h passes/../preasm/macro_handler.h \
 passes/utils_first_pass.h utils/tools.h data_stractures/dynamic_arr.h \
 data_stractures/linkedlist.h test.h
dynamic_arr.o: data_stractures/dynamic_arr.c \
 data_stractures/dynamic_arr.h data_stractures/../utils/tools.h \
 data_stractures/../utils/../utils/errors.h
linkedlist.o: data_stractures/linkedlist.c data_stractures/linkedlist.h \
 data_stractures/../utils/tools.h \
 data_stractures/../utils/../utils/errors.h
lexer.o: lexer/lexer.c lexer/lexer.h lexer/../preasm/macro_handler.h \
 lexer/../preasm/../data_stractures/linkedlist.h \
 lexer/../preasm/../utils/tools.h \
 lexer/../preasm/../utils/../utils/errors.h lexer/../utils/tools.h
firstpass.o: passes/firstpass.c passes/../passes/firstpass.h \
 passes/../passes/utils_first_pass.h \
 passes/../passes/../data_stractures/dynamic_arr.h \
 passes/../passes/../data_stractures/linkedlist.h \
 passes/../passes/../utils/tools.h \
 passes/../passes/../utils/../utils/errors.h \
 passes/../passes/../lexer/lexer.h \
 passes/../passes/../lexer/../preasm/macro_handler.h \
 passes/../passes/../lexer/../preasm/../data_stractures/linkedlist.h \
 passes/../passes/../lexer/../preasm/../utils/tools.h \
 passes/../passes/../preasm/macro_handler.h \
 passes/../passes/utils_first_pass.h passes/../lexer/lexer.h \
 passes/../utils/errors.h
utils_first_pass.o: passes/utils_first_pass.c \
 passes/../passes/utils_first_pass.h \
 passes/../passes/../data_stractures/dynamic_arr.h \
 passes/../passes/../data_stractures/linkedlist.h \
 passes/../passes/../utils/tools.h \
 passes/../passes/../utils/../utils/errors.h \
 passes/../data_stractures/linkedlist.h
macro_handler.o: preasm/macro_handler.c preasm/../preasm/macro_handler.h \
 preasm/../preasm/../data_stractures/linkedlist.h \
 preasm/../preasm/../utils/tools.h \
 preasm/../preasm/../utils/../utils/errors.h \
 preasm/../data_stractures/linkedlist.h
preasm.o: preasm/preasm.c preasm/../utils/tools.h \
 preasm/../utils/../utils/errors.h preasm/../data_stractures/linkedlist.h \
 preasm/../preasm/macro_handler.h \
 preasm/../preasm/../data_stractures/linkedlist.h \
 preasm/../preasm/../utils/tools.h preasm/../preasm/preasm.h \
 preasm/../preasm/../preasm/macro_handler.h preasm/../utils/errors.h
errors.o: utils/errors.c utils/../utils/errors.h
tools.o: utils/tools.c utils/../utils/tools.h \
 utils/../utils/../utils/errors.h utils/../preasm/macro_handler.h \
 utils/../preasm/../data_stractures/linkedlist.h \
 utils/../preasm/../utils/tools.h utils/../utils/errors.h
test.o: test.c lexer/lexer.h lexer/../preasm/macro_handler.h \
 lexer/../preasm/../data_stractures/linkedlist.h \
 lexer/../preasm/../utils/tools.h \
 lexer/../preasm/../utils/../utils/errors.h test.h
 
%.o:
	$(CC) $(CFLAGS) -c $< -o $(OBJ_DIR)/$@
 
clean:
	rm -rf $(BUILD_DIR)
 
build_env:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJ_DIR)
