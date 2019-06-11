#fisier folosit pentru compilarea serverului&clientului TCP iterativ

all:
	gcc earthServTcpIt.c -o earthServTcpIt
	gcc marsServTcpIt.c -o marsServTcpIt
	gcc earthCliTcpIt.c -o earthCliTcpIt
	gcc marsCliTcpIt.c -o marsCliTcpIt
	gcc satelit.c -o satelit
clean:
	rm -f *~cliTcpIt servTcpIt
