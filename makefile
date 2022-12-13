all:  Balcao Cliente Medico
	 

Balcao: Balcao.o
	gcc Balcao.o -o Balcao -pthread

Balcao.o: Balcao.c Balcao.h Cliente.h
	gcc -c Balcao.c

Cliente: Cliente.o
	gcc Cliente.o -o Cliente

Cliente.o: Cliente.c Cliente.h
	gcc -c Cliente.c

Medico: Medico.o
	gcc Medico.o -o Medico

Medico.o: Medico.c Medico.h
	gcc -c Medico.c
clean:
	rm *.o Balcao Cliente
	rm -f /tmp/*_fifo
	

