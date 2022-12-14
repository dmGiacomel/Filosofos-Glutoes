#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <syscall.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

volatile int *vezes_comido, *vezes_pensado; 

//argumento para a função das threads
typedef struct infos_filosofo {
    int indice_filosofo;
    int n_filosofos;
    sem_t *semaforos;
    sem_t *controle_dos_garfo_pegar;
    sem_t *controle_dos_garfo_devolver;
    int timeout;
} infos_filosofo;

//simula o filósofo pensando pausando a thread por um tempo randômico
void pensar(int filosofo){
    printf("Tô pensando... - filósofo %d\n", filosofo);

    unsigned int seed = time(NULL);
    //int random = rand_r(&seed)/(RAND_MAX/(251)+1);
    int random = rand_r(&seed) % (filosofo + 10);
    printf("random = %d\n", random);
    usleep(random);
}

//posicao n do vetor garfos é o garfo à esquerda do filósofo n
//posicao (n+1)%n_filosofos é o garfo à direita do filósofo n
//o filósofo pega o garfo da esquerda, pega o garfo da direita e come por um tempo
void comer(int filosofo, sem_t *controle_dos_garfo_pegar, sem_t *controle_dos_garfo_devolver, sem_t *garfo_esquerda, sem_t *garfo_direita){

    //SEÇÃO CRÍTICA - pegar os dois garfos deve ser atômico para evitar deadlock
    //sem_wait(controle_dos_garfo_pegar);
        sem_wait(garfo_esquerda);
        sem_wait(garfo_direita);
    //sem_post(controle_dos_garfo_pegar);

    printf("- Tô comendo... - filósofo %d\n", filosofo);
    //usleep(300 * filosofo);
    unsigned int seed = time(NULL);
    //int random = rand_r(&seed)/(RAND_MAX/(251)+1);
    int random= rand_r(&seed) % (filosofo + 10);
    printf("random = %d\n", random);
    usleep(random);

    //SEÇÃO CRÍTICA - devolver os dois garfos deve ser atômico, para evitar deadlocks
    sem_wait(controle_dos_garfo_devolver);
        //printf("Comer do filósofo %d chegou aqui\n", filosofo);
        sem_post(garfo_esquerda);
        sem_post(garfo_direita);
    sem_post(controle_dos_garfo_devolver);

}

//posicao n do vetor garfos é o garfo à esquerda do filósofo n
//posicao (n+1)%n_filosofos é o garfo à direita do filósofo n
//ROTINA DO FILÓSOFO
//recebe como parâmetro o número do filósofo e a referência pros mutex dos garfos
void* pensarEComer(void *infos){

    int timeout = ((infos_filosofo*)infos)->timeout;
    int filosofo = ((infos_filosofo*)infos)->indice_filosofo;
    int n_filosofos = ((infos_filosofo*)infos)->n_filosofos;
    sem_t *garfos = ((infos_filosofo*)infos)->semaforos;
    sem_t *controle_dos_garfo_pegar = ((infos_filosofo*)infos)->controle_dos_garfo_pegar;
    sem_t *controle_dos_garfo_devolver = ((infos_filosofo*)infos)->controle_dos_garfo_devolver;

    time_t start = time(NULL);

    int value_garfos;

    //for(int i = 0; i < n_filosofos; i++){
    //    printf("garfo[%d] = %d\n", i, sem_getvalue(&garfos[i], &value_garfos));
    //}

    while((int)(time(NULL) - start) < timeout){
        //printf("---------------- ITERAÇÃO %d DO FILÓSOFO %d----------------------\n", i, filosofo);
        pensar(filosofo);
        vezes_pensado[filosofo]++;
        comer(filosofo, controle_dos_garfo_pegar, controle_dos_garfo_devolver, &garfos[filosofo], &garfos[(filosofo + 1) % n_filosofos]);
        vezes_comido[filosofo]++;
    }
}

int main(int argc, char **argv){

    if(argv[1] == NULL){
        printf("Número de filósofos inválido.\n");
        exit(1);
    }

    int n_filosofos = atoi(argv[1]);
    int timeout = atoi(argv[2]);
    vezes_comido = (int*)calloc(sizeof(int), n_filosofos);
    vezes_pensado = (int*)calloc(sizeof(int), n_filosofos);

    pthread_t filosofos[n_filosofos];
    sem_t *garfos;
    sem_t *controle_dos_garfo_pegar, *controle_dos_garfo_devolver;

    //como os semáforos não são globais, devem estar ou em região de memória compartilhada ou em heap
    garfos = (sem_t*)malloc(sizeof(sem_t) * n_filosofos);
    controle_dos_garfo_devolver = (sem_t*)malloc(sizeof(sem_t));
    controle_dos_garfo_pegar = (sem_t*)malloc(sizeof(sem_t));

    //inicialização dos semáforos 
    for (int i = 0; i < n_filosofos; i++)
        sem_init(&garfos[i], 0, 1);

    //inicialização do controle dos garfo
    sem_init(controle_dos_garfo_pegar, 0, 1);
    sem_init(controle_dos_garfo_devolver, 0, 1);

    //aloca o vetor de argumentos para a função das threads
    infos_filosofo *infos = (infos_filosofo*)malloc(sizeof(infos_filosofo) * n_filosofos); 

    //starta as threads passando o índice do filósofo, número de filósofos, 
    //vetor de semáforos e o mutex que controla o pegar e largar dos garfos
    for (int i = 0; i < n_filosofos; i++){
        infos[i] = (infos_filosofo){i, n_filosofos, garfos, controle_dos_garfo_pegar, controle_dos_garfo_devolver, timeout};
        pthread_create(&filosofos[i], NULL, &pensarEComer, (void *)&infos[i]);
    }

    //destrói os semáforos dos garfos
    //join nas threads dos filósofos
    for(int i = 0; i < n_filosofos; i++){
        pthread_join(filosofos[i], NULL);
        sem_destroy(&garfos[i]);
    }    

    //destrói os semáforos de controle dos garfos
    sem_destroy(controle_dos_garfo_pegar);
    sem_destroy(controle_dos_garfo_devolver);

    for(int i = 0; i < n_filosofos; i++){
        printf("Vezes que o filosofo %d comeu: %d\n", i, vezes_comido[i]);
        printf("Vezes que o filosofo %d pensou: %d\n", i, vezes_pensado[i]);
        printf("\n");
    }

    return 0;
}