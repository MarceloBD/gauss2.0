master: master.c slave
	mpicc master.c -o master
slave: slave.c
	mpicc slave.c -o slave
