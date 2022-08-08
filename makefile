CC=g++ -g
OBJS=NFA.o DFA.o utils.o regexp.o
regexp.exe:$(OBJS)
	$(CC) $(OBJS) -o regexp.exe
DFA.o:DFA.cpp
	$(CC) DFA.cpp -c -o DFA.o
NFA.o:NFA.cpp
	$(CC) NFA.cpp -c -o NFA.o
utils.o:utils.cpp
	$(CC) utils.cpp -c -o utils.o
regexp.o:regexp.cpp
	$(CC) regexp.cpp -c -o regexp.o
clean:
	del $(OBJS) regexp.exe