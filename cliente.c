#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semaforos.h"
#include "estructuras.h"
#include "memoriaCompartida.h"
#include "dataUsuarios.h"

struct infoUsuario menu(struct infoUsuario usuario);
int menuSesionIniciada(struct infoUsuario dataUsu);

int posUsuario(int idUsuario){
  struct Usuarios *usuarios = crearMemComUsuarios();
  for (int i = 0; i < usuarios->cabeza; i++) {
    if (idUsuario == usuarios->idUsuarios[i]) {
      return i;
    }
  }
  return -1;
}

void registarTransaccion(int idUsuario, int idTransaccion, double montoAntes, double montoDespues){
  struct Transacciones *transacciones = crearMemComTransacciones(); //agregar mutex transacciones
  transacciones->idTransaccion[transacciones->cabeza]=idTransaccion;
  transacciones->idUsuarios[transacciones->cabeza]=idUsuario;
  transacciones->montoAntes[transacciones->cabeza]=montoAntes;
  transacciones->montoDespues[transacciones->cabeza]=montoDespues;
  transacciones->cabeza++;
}

int valNumPositivo(double monto){
  if (monto<0) {
    return 1;
  }
  return 0;
}

struct infoUsuario depositar(struct infoUsuario dataUsu){
  struct Usuarios *usuarios = crearMemComUsuarios();

  double montoDepositado, montoDespues;
  int pos;
  printf("Ingrese monto a despositar\n");
  scanf("%lf", &montoDepositado);
  if (valNumPositivo(montoDepositado)) {
    printf("No se aceptan valores negativos\n");
    return dataUsu;
  }

  montoDespues = dataUsu.monto+montoDepositado;
  registarTransaccion(dataUsu.idUser, 1, dataUsu.monto, montoDespues);
  dataUsu.monto+=montoDepositado;
  usuarios->monto[posUsuario(dataUsu.idUser)] = dataUsu.monto;
  enviarInfoServer(dataUsu, 2);
  return dataUsu;
}

struct infoUsuario retirar(struct infoUsuario dataUsu){
  struct Usuarios *usuarios = crearMemComUsuarios();

  double montoRetirado, montoDespues;
  int pos;
  printf("Ingrese monto a retirar\n");
  scanf("%lf", &montoRetirado);
  if (valNumPositivo(montoRetirado)) {
    printf("No se aceptan valores negativos\n");
    return dataUsu;
  }

  montoDespues = dataUsu.monto-montoRetirado;
  if (montoDespues<0) {
    printf("No se puede completar la accion\n");
  }else{
    registarTransaccion(dataUsu.idUser, 2, dataUsu.monto, montoDespues);
    dataUsu.monto-=montoRetirado;
    usuarios->monto[posUsuario(dataUsu.idUser)] = dataUsu.monto;
    enviarInfoServer(dataUsu, 3);
  }


  return dataUsu;
}

int archivoTransacciones(struct infoUsuario dataUsu){
  struct Transacciones *transacciones = crearMemComTransacciones();
  FILE *fichero;
  char nombreArchivo[119] = "";
  strcat(nombreArchivo, dataUsu.user);
  strcat(nombreArchivo, "-Transacciones.txt");
  fichero = fopen(nombreArchivo, "w+");
  fputs("Usuario: ", fichero);
  fputs(dataUsu.user, fichero);
  fputs("\nTransacciones: \n", fichero);
  fputs("TipoTransaccion\t|Monto Antes\t|Monto Despues\n", fichero);
  for (int i = 0; i < transacciones->cabeza; i++) {
    if(transacciones->idUsuarios[i]==dataUsu.idUser){
      if (transacciones->idTransaccion[i]==1) {
        fputs("Deposito\t\t\t\t", fichero);
      }else{
        fputs("Retiro\t\t\t\t", fichero);
      }
      fprintf(fichero, "|$%0.2lf\t\t\t\t", transacciones->montoAntes[i]);
      fprintf(fichero, "|$%0.2lf\n", transacciones->montoDespues[i]);
    }
  }
  fprintf(fichero, "Monto actual: $%0.2lf\n", dataUsu.monto);


  fclose(fichero);
  return 0;
}

int menuSesionIniciada(struct infoUsuario dataUsu){
  int opcion;

  while (1) {

    printf("Tu saldo actual es: %.2lf\n", dataUsu.monto);
    printf("Ingrese una opcion\n1.-Depositar\n2.-Retiro\n3.-Generar Archivo de Transacciones\n4.-Cerrar Sesion\n");
    scanf("%d", &opcion);

    switch (opcion) {
      case 1:
        dataUsu = depositar(dataUsu);

      break;
      case 2:
        dataUsu = retirar(dataUsu);

      break;
      case 3:
        archivoTransacciones(dataUsu);
      break;
      case 4:
        menu(dataUsu);
      break;
    }
  }


  return opcion;
}

struct infoUsuario menu(struct infoUsuario usuario){
  usuario.idUser = -1;
  int opcion;

  printf("Ingrese una opcion\n1.-Iniciar sesion\n2.-Crear cuenta\n3.-Salir\n");
  scanf("%d", &opcion);

  while (usuario.idUser==-1) {
    switch (opcion) {
      case 1:
        usuario = iniciarSesion(getDataUsu(usuario.idHilo),1);
        enviarInfoServer(usuario,1);
      break;
      case 2:
        usuario = crearCuenta(getDataUsu(usuario.idHilo),1);
        enviarInfoServer(usuario,0);
        printf("Inicia Sesion\n");
        usuario = iniciarSesion(getDataUsu(usuario.idHilo),1);
        enviarInfoServer(usuario,1);
      break;
      case 3:
        enviarInfoServer(usuario,5);
        exit(0);
      break;
    }
  }
  menuSesionIniciada(usuario);

  return usuario;
}

int main(int argc, char const *argv[]) {
  struct infoUsuario usuario;
  int mutexEspera = getMutexEspera();
  struct infoUsuario *infoUsuSM = crearMemComInfoUsuario();

  setSemVal(mutexEspera, 2, 0);// Asignar tipo de usurio
  up(mutexEspera, 0);//Senal conexion
  down(mutexEspera, 1);

  down(mutexEspera, 3);//SM infoUsuario ocupada

  usuario.idHilo = infoUsuSM->idHilo;

  up(mutexEspera, 3);//SM infoUsuario liberada

  printf("Ha sido conectado al hilo %d\n", usuario.idHilo);
  menu(usuario);




  return 0;
}
