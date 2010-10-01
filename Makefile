a.out: clean
	g++ main.cc  matrix.cc  -W -Wall  -Werror -Wshadow -g
clean:
	\rm -rf a.out *.o
