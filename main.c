#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define MAX 5
#define escalaHoras 0.001

int begin = 0, end = 0, size = 0, fila[MAX+1];
pthread_mutex_t travaFila;

int vazio(){
  return !size;
}

void floatdelay ( float delay)
{
    clock_t start;
    clock_t stop;

    start = clock();
    stop = clock();
    while ( ( (float)( stop - start)/ CLOCKS_PER_SEC) < delay) {
        stop = clock();
    }

}

void enqueue(int cliente){

  if (size < MAX+1){
    if (vazio())
      printf("O cliente %d acorda o barbeiro e senta em sua cadeira.\n", cliente);
    else
      printf("O cliente %d senta em uma das cadeiras vagas da sala de espera.\n", cliente);

    fila[end] = cliente;
    end = (end+1) % (MAX+1);
    size++;
  }
  else
    printf("Todas as cadeiras estavam ocupadas, o cliente %d foi embora.\n", cliente);
}

int denqueue(){
  if (!vazio()){
    srand(time(NULL));
    int tempoCorte = (20 + (rand() % 11));
    floatdelay(tempoCorte * escalaHoras);
    printf("O barbeiro terminou de cortar o cabelo do cliente %d em %d min, que vai embora.\n", fila[begin], tempoCorte);

    int cliente = fila[begin];
    begin = (begin+1) % (MAX+1);
    size--;

    if (vazio())
      puts("Não há clientes para serem atendidos, o barbeiro vai dormir.");
    else
      printf("O cliente %d senta na cadeira do barbeiro.\n", fila[begin]);

    return cliente;
  }else
    return -1;
}

void* cliente(void* argumento){
  unsigned long idCliente = *((unsigned long*) argumento);

  pthread_mutex_trylock(&travaFila);
  enqueue(idCliente);
  pthread_mutex_unlock(&travaFila);
}

void* barbeiro(void* argumento){
  for ( ; ; ){
    pthread_mutex_trylock(&travaFila);
    denqueue();
    pthread_mutex_unlock(&travaFila);
  }
}

int main(){
  pthread_mutex_init(&travaFila, NULL);

  int i;
  pthread_t threadBarbeiro, threadCliente;

  pthread_create(&threadBarbeiro, NULL, barbeiro, NULL);
  srand(time(NULL));
  int tempoLimite = 30;
  int chegadaPrimeiro = 0;
  int tempoFaltante = 0;
  int tempoDoDia = 480;
  int clientesAtendidosPDia = 0;
  int tempoTrabalhado = 0;
  int clientesAtendidos[5];
  for(int j=1;j<6;++j){
    for (i = 1 ; tempoTrabalhado < 480 ; ++i){
      chegadaPrimeiro = (rand() % tempoLimite +1);
      printf("Tempo para chegar o proximo cliente: %d. \n", chegadaPrimeiro);
      floatdelay(chegadaPrimeiro * escalaHoras);
      pthread_create(&threadCliente, NULL, cliente, &i);
      clientesAtendidosPDia++;
      tempoFaltante = tempoLimite - chegadaPrimeiro;
      floatdelay(tempoFaltante * escalaHoras);
      tempoTrabalhado += 30;
    }
    printf("O dia %d encerrou. Clientes atendidos no dia: %d\n",j, clientesAtendidosPDia);
    clientesAtendidos[j] = clientesAtendidosPDia;
    clientesAtendidosPDia = 0;
    tempoTrabalhado = 0;
  }
  printf("A semana acabou, hora de descansar.\n");

  int soma = 0;
  for(i = 0; i < 5; i++ ){
    soma += clientesAtendidos[i];
  }
  float media = soma /5;

  printf("A media de clientes na semana foi de %.2f", media);

  return 0;
}