default: build

build: hash-table

hash-table: main.cpp hash.cpp
	g++ -Wall main.cpp hash.cpp -o hash-table