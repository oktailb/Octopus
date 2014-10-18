CC=gcc
POC_SRC=src/poc/poc.c
POC_OBJ=$(POC_SRC:.c=.o)
POC_NAME=poc
LIB_SRC=src/lib/parmesan.c
LIB_OBJ=$($LIB_SRC:.c=.o)
LIB_NAME=parmesan
CFLAGS=-W -Wall -g3 -Iinclude/lib -Iinclude/poc
LDFLAGS=-lmicrohttpd

all: $(POC_NAME) lib$(LIB_NAME).so

$(POC_NAME): $(POC_OBJ)
	@echo "Compiling POC"
	$(CC) -o $(POC_NAME) $(POC_OBJ) $(LDFLAGS)

lib$(LIB_NAME).so: $(LIB_OBJ)
	@echo "Comiling the Library"
	$(CC) -o lib$(LIB_NAME).so $(LIB_OBJ) $(LDFLAGS) -shared

clean:
	rm -f $(LIB_OBJ) $(POC_OBJ)
	find . -name '*~' -exec rm -f {} \;

fclean: clean
	rm -f $(POC_NAME) lib$(LIB_NAME).so

re: fclean all

.phony: clean fclean all re
