#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>

#define true 1
#define false 0
#define bool int

key_t llave_cola;


// Estructura usada por la cola de mensajes
typedef struct
{
     long tipo;
     int result;
}MENSAJE;

void llave_cola_mensaje()
{
    int llave;
    printf("Ingrese la llave de la cola de mensajes\n");
    scanf("%d",&llave);
    llave_cola = (key_t)llave;	/*Se usa el nombre del programa como generador de la llave. */
}

int main()
{
    long tipo;
    int msgid;
    MENSAJE msgr;

    tipo = 1;
    llave_cola_mensaje();

    if((msgid = msgget(llave_cola, IPC_CREAT | 0666) ) == -1) {
        perror("Error en la cracion de la cola \n");
        exit(-1);
    }

    system("clear");
    while(true)
    {
        msgrcv(msgid, (void *)&msgr, sizeof(MENSAJE), tipo, 0);

        if(msgr.result==0)
        {
            printf("Empate\n");
        }
        else if(msgr.result==1)
        {
            printf("Gano jugador X\n");
        }
        else if(msgr.result==2)
        {
            printf("Gano jugador O\n");
        }
        else if(msgr.result==3)
        {
            printf("Fin del juego\n");
            break;
        }
    }
    return 0;
}

