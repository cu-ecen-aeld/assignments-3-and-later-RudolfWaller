# Nome do aplicativo
TARGET = writer

# Fonte e objetos
SRC = $(TARGET).c
OBJ = $(SRC:.c=.o)

# Compilador e flags
CC = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -Wextra -Werror -O2

# Alvo padrão
.PHONY: all
all: $(TARGET)

# Regras para gerar o binário
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Regras para gerar arquivos .o
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Limpeza
.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJ)

# Informações úteis
.PHONY: info
info:
	@echo "Compilando com CROSS_COMPILE='$(CROSS_COMPILE)'"

