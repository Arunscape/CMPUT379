default:
	clang -g -Wall dragonshell.c dragon.c -o dragonshell

leak:
	valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=valgrind-out.txt \
         ./dragonshell