#define NUMUSU 10
#define NUMPRODUCTOS 10
#define NUM_MAX_PROD_CARRITO 10
#define NUM_MAX_TRANSACCIONES 80

struct infoUsuario{
  int idHilo;
  int idUser;
  char user[100];
  char password[100];
  double monto;
};

struct Usuarios{
  int idUsuarios[NUMUSU];
  char user[NUMUSU][100];
  char password[NUMUSU][100];
  double monto[NUMUSU];
  int cabeza;
};

struct Transacciones{
  int idTransaccion[NUM_MAX_TRANSACCIONES];
  int idUsuarios[NUM_MAX_TRANSACCIONES];
  double montoAntes[NUM_MAX_TRANSACCIONES];
  double montoDespues[NUM_MAX_TRANSACCIONES];
  int cabeza;
};

//1.-Deposito
//2.-Retiro

/*
Usuario: Juanito
Transacciones:
idT montoAntes montoDespues
1          $50 -> $20
2          $20 -> $80
Monto Acutual: $50
*/
