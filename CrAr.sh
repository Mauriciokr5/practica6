#!/bin/bash
rm carritos catalogo infoUsuario mutexEsp proveedores semHilos semHilosAc SHD usuarios && touch carritos catalogo infoUsuario mutexEsp proveedores semHilos semHilosAc SHD usuarios && gcc -o server server.c -lpthread && ./server
