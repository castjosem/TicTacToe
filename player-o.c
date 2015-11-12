#include "lib_sem.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>

#define true 1
#define false 0
#define bool int

#define TAM 3

#define X 1
#define O 2

key_t llave_sem;
key_t llave_mem;


// Funcion que de 2 coordenadas regresa 1 sola, se usa para simular la matriz con un arreglo
int posicion(int i, int j)
{
    return ((i*TAM)+j);
}

void llave_semaforo()
{
    int llave;
    printf("Ingrese la llave de semaforo\n");
    scanf("%d",&llave);
    llave_sem = (key_t)llave;	/*Se usa el nombre del programa como generador de la llave. */
}

void llave_memoria()
{
    int llave;
    printf("Ingrese la llave de memoria\n");
    scanf("%d",&llave);
    llave_mem = (key_t)llave;	/*Se usa el nombre del programa como generador de la llave. */
}


void reiniciar_juego(int **tab, int *semid,int *shmid)
{
    //Semaforos
    if( ((*semid) = abrir_sem(llave_sem, 2) ) == -1){
 	   perror("Error al abrir los semáforos") ;
	    exit(-1);
	}

    //Memoria compartida
	(*shmid) = shmget(llave_mem, ((TAM*TAM)*sizeof(int))+sizeof(int) , IPC_CREAT | 0666);
    if((*shmid) == -1)
    {
        perror("Error en la creación de una memoria compartida");
        exit(-1);
    }

    (*tab) = (int *)shmat((*shmid), 0, 0);
    if((*tab) == (int *)-1 )
    {
        perror(" La memoria compartida no está bien vinculada");
        exit(-1);
    }
}

void imprimir(int **tab)
{
    int i, j;
    for(i=0; i<TAM; i++)
    {
        printf("\n");
        for(j=0; j<TAM; j++)
        {
            if((*tab)[posicion(i,j)]==X)
                printf("X\t");
            else if((*tab)[posicion(i,j)]==O)
                printf("O\t");
            else
                printf("-\t");
        }
    }
    printf("\n");
}


void jugar(int **tab)
{
    int i,j,pos;
    bool bandera = true;

    while(bandera)
    {
        printf("\nSeleccione la posicion i , j  separadas por un espacio\n");
        scanf("%d",&i);
        scanf("%d",&j);
        i -= 1;
        j -= 1;

        if(i<TAM && j< TAM && i>=0 && j>=0)
        {
            if((*tab)[posicion(i,j)]!=0)
                    printf("\nPosicion ocupada: Elije otra\n");
            else
            {
                (*tab)[posicion(i,j)] = O;
                bandera = false;
            }
        }
    }
}

int main()
{
    int i, shmid;
    int semid;
    int jugadorX = 0;
    int jugadorO = 0;
    bool *continuar;
    int *tab;
    llave_semaforo();
    llave_memoria();

    reiniciar_juego(&tab,&semid,&shmid);

    //Se utiliza continuar como bandera para saber cuando terminar el proceso O
    //continuar apunta a una casilla extra del arreglo del tablero de juego.
    continuar = &tab[(TAM*TAM)+1];
    system("clear");

    //Se bloquea a O en espera de X
    wait_sem(semid,jugadorO);

    while((*continuar))
    {
        system("clear");
        imprimir(&tab);
        jugar(&tab);
        system("clear");

        //Se despierta a X
        signal_sem(semid,jugadorX);

        //Se bloquea a O
        wait_sem(semid,jugadorO);
    }
    return 0;
}
