#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>

//deve utilizar rand_r para gerar valores aleatorios de modo thread-safe

sem_t semaf;

void pegarGarfo(int index){
    sem_wait(garfos[index]);
    printf("pegou garfo %d\n", index);
}

void devolverGarfo(int index){
    sem_post(garfos[index]);
    printf("devolveu o garfo %d\n", index);
}

//simula o filósofo pensando pausando a thread por um tempo randômico
// trocar função por só sleep(n)
/*void pensar(){

}*/

//o filósofo pega o garfo da esquerda, pega o garfo da direita e come por um tempo
void comer(int i1, int i2){
    sem_wait(&semaf);
    // --------------------
        sem_wait(garfos[i1]);
        sem_wait(garfos[i2]);
    // --------------------
    sem_post(&semaf);

    unsigned int *seed = srandom(time(NULL));
    int sleep_time = r_rand(seed);
    sleep(sleep_time);

    sem_wait(&semaf);
    // --------------------
        sem_post(garfos[i1]);
        sem_post(garfos[i2]);
    // --------------------
    sem_post(&semaf);
}

//posicao n do vetor garfos é o garfo à esquerda do filósofo n
//posicao (n+1)%n_filosofos é o garfo à direita do filósofo n
//ROTINA DO FILÓSOFO
void pensarEComer(int *filosofo, int n_filosofos, int *garfos){
    // trocar pensar por sleep
    // pensar();
    comer();
}

int main(int argc, char **argv){
    sem_init(&semaf, 0, 1);
    int n_filosofos = atoi(argv[1]);

    if(n_filosofos <= 0){
        printf("Não existo logo não penso >:(");
        exit(1);
    }

    sem_t garfos[n_filosofos];
    for(int i = 0; i < n_filosofos; i++){
        // vetor preenchido por semáforos binários (mutex), representando cada garfo
        sem_init(&garfos[n_filosofos], 0, 1);
    }

    printf("Número de Filósofos: %d\n", n_filosofos);

    while(1){
        for(int i = 0; i < n_filosofos; i++){
            pthread_create(&garfos[i], NULL, &pensarEComer, NULL);
        }
    }

    return 0;
}
