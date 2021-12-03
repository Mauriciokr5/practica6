#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<sys/shm.h>

#define PERMISOS 0666
#define MAXHILOS 10

int creaSemaforo(key_t llave, int num,int valor_inicial){
   int semid=semget(llave,num,IPC_CREAT|PERMISOS);
   if(semid==-1){
      return -1;
   }
   for (int i = 0; i < num; i++) {
     semctl(semid,i,SETVAL,valor_inicial);
   }
   return semid;
}

int getSemaforo(key_t llave, int num){
   int semid=semget(llave,num,IPC_CREAT|PERMISOS);
   if(semid==-1){
      return -1;
   }
   return semid;
}

void down(int semid, int pos){
  //{posicionSem,Operacion,Flag}
   struct sembuf op_p[]={pos,-1,0};
   semop(semid,op_p,1);//1 por un solo semaforo
}

void up(int semid, int pos){
   struct sembuf op_v[]={pos,+1,0};
   semop(semid,op_v,1);
}

int getSemVal(int semid, int pos){
   return semctl(semid,pos,GETVAL);
}

int setSemVal(int semid, int pos, int val){
   return semctl(semid,pos,SETVAL,val);
}

int crearMutexEspera(){
  int semid = creaSemaforo(ftok("mutexEsp",'a'), 6, 0);
  for (int i = 3; i < 6; i++){
    up(semid, i);
    //printf("semid %d\n", getSemVal(semid, i));
  }

  return semid;
}
/*
[0] Servidor espera cliente
[1] Cliente/Proveedor espera respuesta del hilo Servidor
[2] Es Cliente o Proveedor (0.-cliente, 1.-proveedor)
[3] Lectura/Escritura SM infoUsuario
[4] Lectura/Escritura SM Usuarios
[5] Lectura/Escritura SM Transacciones
*/

int getMutexEspera(){
  return getSemaforo(ftok("mutexEsp",'a'),6);// 0 en espera | 1 ocupado
}

int crearSemHilos(){
  return creaSemaforo(ftok("semHilos",'j'), MAXHILOS, 0);
}

int getSemHilos(){
  return getSemaforo(ftok("semHilos",'j'),MAXHILOS);
}

/*
val sem:
0 .- crearCuenta
1 .- iniciarSesion
2 .- agregarProducto Proveedor
3 .- eliminarProducto Proveedor
4 .- comprar Cliente

*/

int crearSemHilosAccion(){
  return creaSemaforo(ftok("semHilosAc",'j'), MAXHILOS, 0);
}

int getSemHilosAccion(){
  return getSemaforo(ftok("semHilosAc",'j'),MAXHILOS);
}

int crearSemHilosDisponibles(){
  return creaSemaforo(ftok("SHD",'m'), MAXHILOS, 0);
}

int getSemHilosDisponibles(){
  return getSemaforo(ftok("SHD",'m'),MAXHILOS);
}

int hilosDisponible(int semid){
  for (int i = 0; i < MAXHILOS; i++)
    if (!getSemVal(semid, i))
      return i;
  return -1;
}
