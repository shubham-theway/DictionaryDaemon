compile:hashtable.c client.c keywords 
	touch /tmp/10 /tmp/20
	chmod 777 /tmp/10
	chmod 777 /tmp/20
	gcc -w -o hash.o hashtable.c
	gcc -w -o client.o client.c
run:hash.o keywords 
	-./hash.o keywords
