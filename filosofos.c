#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>

//deve utilizar rand_r para gerar valores aleatorios de modo thread-safe

void pegarGarfo();

//simula o filósofo pensando pausando a thread por um tempo randômico
void pensar(){

}

//o filósofo pega o garfo da esquerda, pega o garfo da direita e come por um tempo
void comer(){

}

//posicao n do vetor garfos é o garfo à esquerda do filósofo n
//posicao (n+1)%n_filosofos é o garfo à direita do filósofo n
//ROTINA DO FILÓSOFO
void pensarEComer(int *filosofo, int n_filosofos, int *garfos){
    // trocar pensar por sleep
    pensar();
    comer();
}

int main(int argc, char **argv){

    int n_filosofos = atoi(argv[1]);
    sem_t garfos[n_filosofos];
    for(int i = 0; i < n_filosofos; i++){
        // vetor preenchido por semáforos binários (mutex), representando cada garfo
        sem_init(&garfos[n_filosofos], 0, 1);
    }

    srandom(time(NULL));

    printf("Número de Filósofos: %d\n", n_filosofos);

    return 0;
}
