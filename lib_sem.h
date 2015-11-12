/******************************************
* fichero:  lib_sem.h                     *
* Objetivo: Usar como fichero de cabecera *
******************************************/
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>
/* Los semáforos se crearan con los mismos permisos */
#define PERMISOS 0666 

/*************************** prototipo de funciones *******/
int  abrir_sem(key_t,  int );
void init_todos(int, ushort [] );
void init_sem(int, int , int );
void wait_sem(int , int );                                  
void signal_sem(int , int );
void borra_sem(int );
int valor_sem(int, int);

/******************** Abre o crea un conjunto de semáforos ***/
int abrir_sem(key_t llave, int numsem)
{
	int semid = semget(llave, numsem, IPC_CREAT | PERMISOS);
   if(semid == -1) {
     perror("Error al crear el semáforo");
     exit(-1);
   }
   return (semid);
}
/************************* Inicializa todos los semáforos  ***/
void init_todos(int semid, ushort array_ini[])
{
  semctl(semid, 0, SETALL, array_ini);
}

/************************** Inicializa un solo semáforo  ***/
void init_sem(int semid, int nsem, int ini)
{
 semctl(semid, nsem, SETVAL, ini);
}

/********************* Operación Wait  sobre un semáforo ***/
void wait_sem(int semid, int nsem)
{
struct sembuf op_w = {nsem, -1, 0};

  semop(semid, &op_w, 1);
}

/********************* Operación Signal sobre un semáforo ***/
void signal_sem(int semid, int nsem)
{
	struct sembuf op_s = {nsem, 1, 0};

	semop(semid, &op_s, 1);
} 

/************************Borrar un conjunto de semáforos ***/
void borra_sem(int semid)
{
	semctl(semid, 0, IPC_RMID, 0);
}

/********************** Valor del semáforo ***************/
int valor_sem(int semid, int nsem)
{
	return ( semctl(semid, nsem, GETVAL,0));
}

