#include "lib_sem.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <fcntl.h>


#define true 1
#define false 0
#define bool int

#define TAM 3

#define X 1
#define O 2

key_t llave_sem;
key_t llave_mem;
key_t llave_cola;

// Estructura usada por la cola de mensajes
typedef struct
{
     long tipo;
     int result;
}MENSAJE;

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

void llave_cola_mensaje()
{
    int llave;
    printf("Ingrese la llave de la cola de mensajes\n");
    scanf("%d",&llave);
    llave_cola = (key_t)llave;	/*Se usa el nombre del programa como generador de la llave. */
}

/* Crea la memoria compartida, los semaforos, la cola de mensajes e inicializa
*  la matriz con 0
*/
void reiniciar_juego(int **tab, int *semid, int *shmid, int *msgid)
{
    int i, j;

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

    if( ((*semid) = abrir_sem(llave_sem, 2) ) == -1)
    {
 	   perror("Error al abrir los semáforos") ;
	    exit(-1);
    }


    if(((*msgid) = msgget(llave_cola, IPC_CREAT | 0666) ) == -1)
    {
        perror("Error en la creacion de la cola \n");
        exit(-1);
    }

    for(i=0; i<TAM; i++)
    {
        for(j=0; j<TAM; j++)
        {
            (*tab)[posicion(i,j)] = 0;
        }
    }

    for(i=0; i<TAM; i++)
    {
        printf("\n");
        for(j=0; j<TAM; j++)
        {
            printf("%d\t",(*tab)[posicion(i,j)]);
        }
    }
    printf("\n");

    (*tab)[(TAM*TAM)+1] = true;
}

void imprimir(int **tab)
{
    int i, j;
    for(i=0; i<TAM; i++)
    {
        printf("\n");
        for(j=0; j<TAM; j++)
        {
            //printf("%d\t",(*tab)[posicion(i,j)]);
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

/* Valida todas las oportunidades que tiene cualquier jugador para ganar excepto
*  los empates
*/
bool validar(int **tab,int jugador)
{
    int i,j;
    bool bandera;

    //Se valida si se gano con una jugada horizontal
    for(i=0;i<TAM;i++)
    {
        bandera = true;
        for(j=0;j<TAM && bandera;j++)
        {
            bandera = ((*tab)[posicion(i,j)]==jugador);
        }
        if(bandera)
            return true;
    }

    //Se valida si se gano con una jugada vertical
    for(i=0;i<TAM;i++)
    {
        bandera = true;
        for(j=0;j<TAM && bandera;j++)
        {
            bandera = ((*tab)[posicion(j,i)]==jugador);
        }
        if(bandera)
            return true;
    }

    //Se valida si se gano con una jugada diagonal nor-oeste  sur-este
    bandera = true;
    for(i=0;i<TAM && bandera;i++)
    {
        bandera = ((*tab)[posicion(i,i)]==jugador);
    }
    if(bandera)
            return true;

    //Se valida si se gano con una jugada diagonal sur-oeste  nor-este
    bandera = true;
    j = 0;
    for(i=TAM-1;i>=0 && bandera ;i--)
    {
        bandera = ((*tab)[posicion(i,j)]==jugador);
        j++;
    }
    if(bandera)
            return true;

    return false;
}

bool validar_empate(int **tab)
{
    int i, j;
    //Se valida que, si todas las posiciones de la matriz estan ocupadas, entonces hubo un empate
    bool bandera = true;
    for(i=0;i<TAM && bandera;i++)
    {
        for(j=0;j<TAM && bandera;j++)
        {
            bandera = ((*tab)[posicion(i,j)]!=0);
        }
    }
    if(bandera)
            return true;
}

/* El jugador puede seleccionar una posicion de la matriz en caso de que este ocupada
*  pedira que seleccione otra, esto lo hará mientras la posicion de la matriz este ocupada
*/
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
	        (*tab)[posicion(i,j)] = X;
                bandera = false;
            }
        }
    }
}

//Se envia un mensaje mediante Cola de Mensajes al proceso Pantalla del resultado del juego
void enviarCola(MENSAJE *msg,int *msgid,int resultado)
{
	(*msg).result = resultado;
	int msgsz = sizeof((*msg)) - sizeof((*msg).tipo);
    msgsnd((*msgid), (void *)&(*msg), msgsz, 0 );
}


int main()
{

    MENSAJE msg;
    int shmid, msgid,msgsz,semid;
    int jugadorX = 0,jugadorO = 0;
    bool empate,ganoX,ganoO,bandera;
    ushort valor_sem[2] = {0, 0};
    int repetir_juego;
    int *tab;
    bool *continuar;
    msg.tipo = 1;
    bandera = true;

    llave_semaforo();
    llave_memoria();
    llave_cola_mensaje();

    reiniciar_juego(&tab,&semid,&shmid,&msgid);

    //Se utiliza continuar como bandera para terminar el proceso O
    //continuar apunta a una casilla extra del arreglo del tablero de juego.
    continuar = &tab[(TAM*TAM)+1];

    while((*continuar))
    {
        //inicializa los semaforos
        init_todos(semid, valor_sem);

        system("clear");
        while(true)
        {
            ganoO = validar(&tab,O);
            if(ganoO)
            {
                enviarCola(&msg,&msgid,2);
                break;
            }

            empate = validar_empate(&tab);
            if(empate)
            {
                enviarCola(&msg,&msgid,0);
                break;
            }

            imprimir(&tab);
            jugar(&tab);
            system("clear");

            ganoX = validar(&tab,X);
            if(ganoX)
            {
                enviarCola(&msg,&msgid,1);
                break;
            }

            empate = validar_empate(&tab);
            if(empate)
            {
                enviarCola(&msg,&msgid,0);
                break;
            }

            //Se despierta a O
            signal_sem(semid,jugadorO);
            //Se bloquea a X
            wait_sem(semid,jugadorX);
        }

        do
        {
            system("clear");
            printf("\nVolver a jugar si=1/no=0:\n");
            scanf("%d",&repetir_juego);
            if(repetir_juego==1||repetir_juego==0)
                bandera = false;
		    else
                printf("\nES 0 ó 1\n");
        }while(bandera);

        //Si NO se va a seguir jugando se cambia la bandera y se termina al proceso O
        //se hace un anuncio de fin de juego en pantalla, se termina el proceso pantalla
        //mediante un mensaje de fin
        if(repetir_juego==0)
        {
            (*continuar) = false;
            signal_sem(semid,jugadorO);
            enviarCola(&msg,&msgid,3);
            break;
        }
        reiniciar_juego(&tab,&semid,&shmid,&msgid);
    }

    shmctl(shmid, IPC_RMID, 0); /* Borramos la memoria compartida */
    return 0;
}
