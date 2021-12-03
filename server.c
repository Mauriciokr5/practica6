#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "semaforos.h"
#include "estructuras.h"
#include "memoriaCompartida.h"

void iniciarMemoriaCompartida(){
  struct Usuarios *usuarios = crearMemComUsuarios();
  usuarios->cabeza = 0;

  struct Transacciones *transacciones = crearMemComTransacciones();
  transacciones->cabeza = 0;
}

void *hiloCliente(void *data){
  struct infoUsuario infoUsu = *((struct infoUsuario *)data);//struct local
  struct infoUsuario *infoUsuSM = crearMemComInfoUsuario();

  int loop = 1;

  int semHilosDisponibles = getSemHilosDisponibles();
  int mutexEspera = getMutexEspera();
  int semHilos = getSemHilos();
  int semHilosAccion = getSemHilosAccion();

  up(semHilosDisponibles, infoUsu.idHilo);//Hilo ocupado
  down(mutexEspera, 3);//SM infoUsuario ocupada
  infoUsuSM->idHilo = infoUsu.idHilo;
  up(mutexEspera, 3);//SM infoUsuario liberada
  up(mutexEspera, 1);//senal ServerConectado

  while (loop) {
    down(semHilos, infoUsu.idHilo);//Espera accion de proveedor

    down(mutexEspera, 3);//SM infoUsuario ocupada
    switch (getSemVal(semHilosAccion, infoUsu.idHilo)) {//nota: agregar monto actual a la notificacion
      case 0:
         printf("Se ha creado la cuenta \"%s\".\n", infoUsuSM->user);
      break;
      case 1:
        printf("El usuario \"%s\" ha iniciado sesion\n", infoUsuSM->user);
      break;
      case 2:
        printf("El cliente \"%s\" ha depositado en su cuenta.\n", infoUsuSM->user);//Deposito
      break;
      case 3:
        printf("El cliente \"%s\" ha hecho un retiro.\n", infoUsuSM->user);//Retiro
      break;
      case 5:
        loop = 0;
      break;
    }
    up(mutexEspera, 3);//SM infoUsuario liberada
  }

  down(semHilosDisponibles, infoUsu.idHilo);
  printf("Hilo %d terminado\n", infoUsu.idHilo);
}

int main(int argc, char const *argv[]) {
  int mutexEspera, semHilos, semHilosDisponibles, semHilosAccion;

  mutexEspera = crearMutexEspera();
  semHilos = crearSemHilos();
  semHilosDisponibles = crearSemHilosDisponibles();
  semHilosAccion = crearSemHilosAccion();
  iniciarMemoriaCompartida();

  while(1){
    sleep(1);
    printf("Esperando Conexion\n");
    down(mutexEspera, 0);//Espera conexion
    struct infoUsuario infoUsu= {hilosDisponible(semHilosDisponibles), 0, "", ""};

    printf("Hilo %d creado\n", infoUsu.idHilo);
    pthread_t hiloClienteD;
    pthread_create(&hiloClienteD, NULL, &hiloCliente, (void *)&infoUsu);
    printf("Se conecto un cliente\n");

  }


  return 0;
}
