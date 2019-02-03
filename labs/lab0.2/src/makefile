# build for strlen library activity
# Cesar Augusto A01153737

all: main.c
	gcc -c -g -w strlib.c -o ../libs/strlib.object
	gcc -c -g -w main.c -o ../libs/main.object
	gcc ../libs/strlib.object ../libs/main.object -o ../bin/a.out

clean: 
	$(RM) ../libs/strlib.object ../libs/main.object ../bin/a.out
