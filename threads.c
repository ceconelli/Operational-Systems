#include <pthread.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>

const size_t numprocs = 4;
double resultados[4];
int size = 13;
int *vetor;

void print_array(){
	for (int i = 0; i < size; i++){
		printf("%d ",vetor[i]);
	}
	printf("\n");
}


void* func(void * id){
	int begin,end;
	
	long ID=(long)id;
	pid_t tid;
	tid = syscall(SYS_gettid);
	printf("Thread: %ld(%d) - Process ID: %5d Pai:%5d\n",ID,tid,getpid(),getppid());
	
	int third_part_index = size/2;
	int second_part_index = third_part_index/2;
	int forth_part_index = (third_part_index + size)/2;
	
	
	switch(ID){
		case 0:
			begin = 0;
			end = second_part_index;
		break;

		case 1:
			begin = second_part_index;
			end = third_part_index;
		break;

		case 2:
			begin = third_part_index;
			end = forth_part_index;
		break;

		case 3:
			begin = forth_part_index;
			end = size;
		break;
	}

	for (int i = begin; i < end; i++){
		vetor[i] = 1 + rand()%20;
	}

		

	pthread_exit(NULL);
}


void* bigger(void * id){

	int max,begin,end;
	long ID=(long)id;

	int third_part_index = size/2;
	int second_part_index = third_part_index/2;
	int forth_part_index = (third_part_index + size)/2;
	
	switch(ID){
		case 0:
			begin = 0;
			end = second_part_index;
		break;

		case 1:
			begin = second_part_index;
			end = third_part_index;
		break;

		case 2:
			begin = third_part_index;
			end = forth_part_index;
		break;

		case 3:
			begin = forth_part_index;
			end = size;
		break;
	}

	max = vetor[begin];

	for (int i = begin; i < end; i++){
		if (vetor[i] > max){
			max = vetor[i];
		}
	}

	resultados[ID] = max;


}


int main(){	
	pid_t tid;
	tid = syscall(SYS_gettid);

	printf("Thread: (%d) - Process ID: %5d Pai:%5d\n",tid,getpid(),getppid());

	srand(time(NULL));

	vetor = (int*) malloc(size * sizeof(int));
	print_array();

	//Idenficador de cada thread
	pthread_t handles[numprocs]; 
	/*Carrega os atributos padrões para criação
	 das threads. Dentre os atributos, estão:
	prioridade no escalonamento e tamanho da pilha.*/
	pthread_attr_t attr; 
	pthread_attr_init(&attr);
	//Cria as threads usando os atributos carregados.
	long i;
	for (i = 0; i < numprocs; i++){
		pthread_create(&handles[i], &attr, func, (void*)i);
	}
	// Espera todas as threads terminarem.
	for (i = 0; i != numprocs; ++i)
		pthread_join(handles[i], NULL); //NULL -> parâmetro de retorno
	/* Soma o resultado de cada thread.
	Observe que cada thread escreve em uma posição
	do vetor de resultados (o que evita inconsistência).*/
	print_array();

	/*------------------------------------------------------------------------------------------------
	------------------------------------------------------------------------------------------------*/

	//Idenficador de cada thread
	pthread_t handles_bigger[numprocs]; 
	/*Carrega os atributos padrões para criação
	 das threads. Dentre os atributos, estão:
	prioridade no escalonamento e tamanho da pilha.*/
	pthread_attr_t attr_bigger; 
	pthread_attr_init(&attr_bigger);
	//Cria as threads usando os atributos carregados.
	long j;
	for (j = 0; j < numprocs; j++){
		pthread_create(&handles_bigger[j], &attr_bigger, bigger, (void*)j);
	}
	// Espera todas as threads terminarem.
	for (j = 0; j != numprocs; ++j)
		pthread_join(handles_bigger[j], NULL); //NULL -> parâmetro de retorno
	
	int max = resultados[0];
	for (int i = 0; i < numprocs; ++i){
		printf("%f ",resultados[i]);
		if (resultados[i] > max){

			max = resultados[i];
		}
	}

	printf("\nMAX: %d\n",max);


	return 0;
}
