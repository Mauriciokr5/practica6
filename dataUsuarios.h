struct infoUsuario getDataUsu(int idHilo){
  struct infoUsuario temp;
  temp.idHilo = idHilo;
  temp.idUser = -1;
  printf("Ingrese nombre de usuario\n");
  scanf("%s", temp.user);
  printf("Ingrese contrasena\n");
  scanf("%s", temp.password);//Aqui se oculta la contrasena con *
  return temp;
}

struct infoUsuario iniciarSesion(struct infoUsuario dataUsu, int tipoUser){//0.-Proveedor, 1.-Cliente
  struct Usuarios *usuarios = crearMemComUsuarios();
  int mutexEspera = getMutexEspera();


  down(mutexEspera, 4);//SM Usuarios ocupada

  for (int i = 0; i < usuarios->cabeza; i++) {
    if (!strcmp(usuarios->user[i],dataUsu.user)) {
      if (!strcmp(usuarios->password[i],dataUsu.password)) {
        dataUsu.idUser = usuarios->idUsuarios[i];
        strcpy(dataUsu.user, usuarios->user[i]);
        dataUsu.monto = usuarios->monto[i];
        break;
      }
    }
  }
  up(mutexEspera, 4);//SM Usuarios liberada

  if (dataUsu.idUser==-1) {
    printf("Datos erroneos\n");
  }
  return dataUsu;
}

struct infoUsuario crearCuenta(struct infoUsuario dataUsu, int tipoUser){
  struct Usuarios *usuarios = crearMemComUsuarios();
  int mutexEspera = getMutexEspera();


  down(mutexEspera, 4);//SM Usuarios ocupada

  usuarios->idUsuarios[usuarios->cabeza] = usuarios->cabeza;
  dataUsu.idUser = usuarios->cabeza;
  strcpy(usuarios->user[usuarios->cabeza], dataUsu.user);
  strcpy(usuarios->password[usuarios->cabeza], dataUsu.password);
  usuarios->monto[usuarios->cabeza] = 0;
  dataUsu.monto = 0;
  usuarios->cabeza++;

  up(mutexEspera, 4);//SM Usuarios liberada



  return dataUsu;
}

void enviarInfoServer(struct infoUsuario dataUsu, int accion){
  struct infoUsuario *infoUsuSM = crearMemComInfoUsuario();
  int mutexEspera = getMutexEspera();
  int semHilosAccion = getSemHilosAccion();
  int semHilos = getSemHilos();

  down(mutexEspera, 3);//SM infoUsuario ocupada

  infoUsuSM->idHilo = dataUsu.idHilo;
  infoUsuSM->idUser = dataUsu.idUser;
  strcpy(infoUsuSM->user, dataUsu.user);
  strcpy(infoUsuSM->password, dataUsu.password);
  infoUsuSM->monto = dataUsu.monto;
  setSemVal(semHilosAccion, dataUsu.idHilo, accion);
  getSemVal(semHilos, dataUsu.idHilo);

  up(semHilos, dataUsu.idHilo);//senal a hilo de accion

  up(mutexEspera, 3);//SM infoUsuario liberada

}
