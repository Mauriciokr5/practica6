
struct infoUsuario *crearMemComInfoUsuario(){
  int shmid;
  struct infoUsuario *infoUsuario;
  key_t llaveInfoUsuario;
  llaveInfoUsuario=ftok("infoUsuario",'l');
  shmid=shmget(llaveInfoUsuario,sizeof(struct infoUsuario),IPC_CREAT|0777);
  infoUsuario=shmat(shmid,0,0);
  return infoUsuario;
}

struct Usuarios *crearMemComUsuarios(){
  int shmid;
  struct Usuarios *usuarios;
  key_t llaveUsuarios;
  llaveUsuarios=ftok("usuarios",'l');
  shmid=shmget(llaveUsuarios,sizeof(struct Usuarios),IPC_CREAT|0777);
  usuarios=shmat(shmid,0,0);
  return usuarios;
}

struct Transacciones *crearMemComTransacciones(){
  int shmid;
  struct Transacciones *transacciones;
  key_t llaveTransacciones;
  llaveTransacciones=ftok("transacciones",'l');
  shmid=shmget(llaveTransacciones,sizeof(struct Transacciones),IPC_CREAT|0777);
  transacciones=shmat(shmid,0,0);
  return transacciones;
}
