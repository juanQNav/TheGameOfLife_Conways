# Variables
CC := gcc
CFLAGS := -I../include
LDFLAGS := -L../lib
LDLIBS := -lopengl32 -lgdi32 -lglfw3dll -lglfw3
SRC_DIR := ../src
BIN_DIR := .
OBJ_DIR := obj

# Lista de archivos fuente
SOURCES := $(wildcard $(SRC_DIR)/*.c)

# Lista de archivos objeto
OBJECTS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))

# Nombre del nuevo ejecutable (cambiar de "the_game_of_life" a lo que desees)
EXECUTABLE := $(BIN_DIR)/the_game_of_life.exe

# Regla para el objetivo principal (el ejecutable)
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJECTS) $(LDLIBS) -mwindows

# Regla para compilar cada archivo fuente a un archivo objeto
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Regla para limpiar los archivos objeto y el ejecutable
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
