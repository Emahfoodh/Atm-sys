# Specify the directory for object files
OBJ_DIR := target

# List of object files
objects := $(addprefix $(OBJ_DIR)/, main.o system.o auth.o sql.o f.o)

atm : $(objects)
	$(CC) -o $@ $^ -lncurses -lsqlite3

# Rule for compiling object files
$(OBJ_DIR)/%.o: src/%.c src/header.h | $(OBJ_DIR)
	$(CC) -c $< -o $@

# Create the object directory if it does not exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean :
	rm -f $(objects)
	rmdir $(OBJ_DIR)
	rm -f atm

analyze : 
	clang --analyze src/*.c