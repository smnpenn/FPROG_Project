all: main tests

.outputFolder:
	mkdir -p out

main: .outputFolder
	clang -std=c++17 -lstdc++ -lm -O3 main.cpp -Wall -Wextra -Werror -o out/main
	./out/main

tests: .outputFolder
	clang -std=c++17 -lstdc++ -lm -O3 tests.cpp -Wall -Wextra -Werror -o out/tests
	./out/tests