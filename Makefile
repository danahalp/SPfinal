symnmf: symnmf.o symnmf.h
	gcc -o symnmf symnmf.o -lm

symnmf.o: symnmf.c symnmf.h
	gcc -c symnmf.c -ansi -Wall -Wextra -Werror -pedantic-errors

clean:
	rm*.o
