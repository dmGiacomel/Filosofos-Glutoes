#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
    pensar();
    comer();
}

int main(int argc, char **argv){

    int n_filosofos = atoi(argv[1]);
    int garfos[n_filosofos];

    srandom(time(NULL));

    printf("Número de Filósofos: %d\n", n_filosofos);

    return 0;
}