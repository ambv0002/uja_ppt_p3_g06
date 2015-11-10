/*******************************************************
Protocolos de Transporte
Grado en Ingeniería Telemática
Dpto. Ingeníería de Telecomunicación
Univerisdad de Jaén

Fichero: cliente.c
Versión: 1.0
Fecha: 23/09/2012
Descripción:
	Cliente de eco sencillo TCP.

Autor: Juan Carlos Cuevas Martínez

*******************************************************/

/* **Pareja de trabajo: Macarena Barselo Vazquez y Begoña Calvo Castillo** */
#include <stdio.h>
#include <winsock.h>
#include <time.h>
#include <conio.h>
#include <string.h>
#include "protocol.h"





int main(int *argc, char *argv[])
{
	SOCKET sockfd; // Definimos un socket, lo llamamos sockfd
	struct sockaddr_in server_in; // Estructura basica para llamadas al sistema y funciones relacionada con direcciones de Internet
	char buffer_in[1024], buffer_out[1024],input[1024],origen[20],destino[20],asunto[30],mensaje[1024]; // Buffers y entrada de datos
	int recibidos=0,enviados=0; // Variables para el envio y recepcion de los datos, ambas iniciadas a 0
	int estado=S_HELO; // Estados de la conexion iniciado a S_HELO
	char option; // Variable condicional para el bucle "do"
	WORD wVersionRequested; // Para trabajar con sockets en Windows
	WSADATA wsaData; // Para trabajar con sockets en Windows
	int err; // Usamos "err" para la comprobacion de errores
    char ipdest[16];// Declaramos la direccion IP destino
	char default_ip[16]="127.0.0.1"; // Direccion IP establecida por defecto
	int n,fin=0,contador=0,cabecera;
	char opcion[5];

	/*Para la fecha y hora*/
	time_t tiempo = time(0);
	struct tm *tlocal = localtime(&tiempo);
	char zona[10];
	char output[128];
	//Inicialización Windows sockets
	wVersionRequested=MAKEWORD(1,1);
	err=WSAStartup(wVersionRequested,&wsaData);
	if(err!=0)
		return(0);

	if(LOBYTE(wsaData.wVersion)!=1||HIBYTE(wsaData.wVersion)!=1)
	{
		WSACleanup();  // Liberar recursos de la liberia
		return(0);
	}
	//Fin: Inicialización Windows sockets

	/* Con "do" se crea el socket en el que se recibiran y se enviaran los datos */

	do{
		sockfd=socket(AF_INET,SOCK_STREAM,0);

		if(sockfd==INVALID_SOCKET) // Si el socket es invalido, mostramos un error por pantalla.
		{
			printf("CLIENTE> ERROR AL CREAR SOCKET\r\n");
			exit(-1);
		} 
		else // Si el socket es valido, mostramos mensaje por pantalla
		{
			printf("CLIENTE> SOCKET CREADO CORRECTAMENTE\r\n");

			// Pedimos la IP de destino, si pulsamos "Enter" introduciremos la IP por defecto indicada en la parte superior del codigo		
			printf("CLIENTE> Introduzca la IP destino (pulsar enter para IP por defecto): ");
			gets(ipdest);

			// Para introducir la IP por defecto al pulsar "Enter"
			if(strcmp(ipdest,"")==0)
				strcpy(ipdest,default_ip);

			// Asignamos los valores a la estructura creada al inicio del codigo, "server_in"
			server_in.sin_family=AF_INET; // Familia de protocolos de Internet
			server_in.sin_port=htons(SMTP_SERVICE_PORT); // Puerto del servidor
			server_in.sin_addr.s_addr=inet_addr(ipdest); // Direccion IP del servidor
			
			estado=S_HELO; // Se reestablece el estado inicial
		
			// establece la conexion de transporte
			if(connect(sockfd,(struct sockaddr*)&server_in,sizeof(server_in))==0)
			{
				printf("CLIENTE> CONEXION ESTABLECIDA CON %s:%d\r\n",ipdest,SMTP_SERVICE_PORT);
			
		
				//Inicio de la máquina de estados

				do{
					switch(estado)
					{

					case S_HELO:
						// Se recibe el mensaje de bienvenida
						sprintf_s (buffer_out, sizeof(buffer_out), "HELO %s%s",ipdest,CRLF);
						break;

					case S_MAIL:
						// establece la conexion de aplicacion 
						printf("CLIENTE> Introduzca su usuario (enter para salir): ");
						gets(origen);
						if(strlen(input)==0)
						{
							sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",SD,CRLF);
							estado=S_QUIT;
						}
						else
						{
						/*Envia y recibe los datos asociados con el destinatario*/
						sprintf_s (buffer_out, sizeof(buffer_out), "MAIL FROM: %s%s",origen,CRLF);

						}
						break;

					case S_RCPT:
						printf("CLIENTE> Introduzca el destinatario (enter para salir): ");
						gets(destino);
						if(strlen(input)==0)
						{
							sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",SD,CRLF);
							estado=S_QUIT;
						}
						else{
						sprintf_s (buffer_out, sizeof(buffer_out), "RCPT TO: %s%s",destino,CRLF);
						}
						break;

				case S_DATA:
						sprintf_s (buffer_out, sizeof(buffer_out), "DATA%s",CRLF);
						estado=S_MENSAJE;
						break;
				case S_MENSAJE:
					/*
					estado mesaje:
					recibo=0
					cuando el usuario ponga el . recibo=1;
					if(recibo==1)
					recv...
					*/
					 //uja_PPT_p3_g06
					 if(contador==0)
					 {
					 strftime(output,128,"%d/%m/%y %H:%M:%S",tlocal);				
					 printf("Introduce el asunto del mensaje:");
					 gets(asunto);
					 sprintf_s(buffer_out, sizeof(buffer_out), "Date: %s GMT:%d%sFrom: <%s>%sSubject: %s%sTo: <%s>%s%s",output,getTimeZone(),CRLF,origen,CRLF,asunto,CRLF,destino,CRLF,CRLF);
					 contador=1;
					}
					else{
					 contador=0;
					 printf("Introduce el mensaje: ");
					 gets(mensaje);
					 
					 // sprintf_s (buffer_out, sizeof(buffer_out), "Date: %s%sFrom: %s%sTo: %s%sSubject: %s%s %s%s%s.%s",fecha,CRLF,origen,CRLF,destino,CRLF,asunto,CRLF,CRLF,mensaje,CRLF,CRLF);
					 sprintf_s (buffer_out, sizeof(buffer_out), "%s%s.%s",mensaje,CRLF,CRLF);
					}
				case S_QUIT:
					sprintf_s (buffer_out, sizeof(buffer_out), "QUIT%s",CRLF);
					fin=1;
						break;
					}/*Cierre del switch*/
			
					//Envio
					if(estado!=S_HELO){
					// Ejercicio: Comprobar el estado de envio

						// sockfd: socket que utilizamos
						// buffer_out: buffer que usamos 
						//(int)strlen(buffer_out): tamaño del buffer con los datos que vamos a enviar (en bytes)
						// 0: flag, funcionamiento normal del protocolo.
						enviados=send(sockfd,buffer_out,(int)strlen(buffer_out),0);

						if(enviados==SOCKET_ERROR || enviados==0)
						{
							if(enviados==SOCKET_ERROR)
							{
							DWORD error=GetLastError();
							printf("CLIENTE> Error %d en el envío de datos\r\n",error);
							estado=S_QUIT;
							}
							else
							{
							printf("CLIENTE> Conexión con el servidor cerrada\r\n");
							estado=S_QUIT;
							}
						}
					}
						
			
						if(estado==S_MENSAJE && contador==1)
						{
							continue; /*Para que no reciba datos del servidor*/
						}
					recibidos=recv(sockfd,buffer_in,512,0);

					if(recibidos<=0) // Si recibimos 0 o -1
					{
						DWORD error=GetLastError();
						if(recibidos<0) // Si recibimos un -1 o SOCKET_ERROR la operacion ha fallado
						{
							printf("CLIENTE> Error %d en la recepción de datos\r\n",error);  // Salimos con "estado=S_QUIT"
							estado=S_QUIT; 
						}
						else // Si recibimos un 0, la conexion ha sido liberada de forma acordada
						{
							printf("CLIENTE> Conexión con el servidor cerrada\r\n");
							estado=S_QUIT;
						
					
						}
					}else{  // Si recibimos la cantidad de bytes enviados
						buffer_in[recibidos]=0x00; // Iniciamos a 0 porque en C los arrays finalizan con el byte 0000 0000
						printf(buffer_in);
						printf(buffer_out);// Imprimimos por pantalla el valor
						//if(estado!=S_DATA && strncmp(buffer_in,OK,2)==0) // Si no estamos en "estado=S_DATA" (DATA es el último estado) y hemos recibido un OK, podemos pasar al siguiente estado
							//estado++;  
						if((estado==S_HELO || estado==S_MAIL || estado==S_RCPT || estado==S_MENSAJE) && strncmp(buffer_in,"2",1)==0)
							estado++;
						
						if(estado==S_DATA && strncmp(buffer_in,"3",1)==0)
							estado++;
						
					}

				}while(fin!=1); // Se realiza el bucle mientras el estado sea distinto a "S_QUIT"
				/*Enviar otro correo*/
					printf("¿Enviar otro correo? Pulsa 1 para continuar, en otro caso, se saldrá\n");
					 gets(opcion);
					 if(opcion=="1")
						 estado=S_MAIL;
					 else
					 {
						estado=S_QUIT;
					 }
			}


			else
			{
				printf("CLIENTE> ERROR AL CONECTAR CON %s:%d\r\n",ipdest,SMTP_SERVICE_PORT); // Muestra el mensaje de error al conectar el cliente con la IP destino y el puerto TCP escogidos
			}

			
			// fin de la conexion de transporte
			closesocket(sockfd);
			
		}	
		do{
		printf("-----------------------\r\n\r\nCLIENTE> Volver a conectar (S/N)\r\n"); // Opcion para volver a realizar una conexion
		option=_getche();
		}while((option!='s' && option!='S') && (option!='n' && option!='N'));
		
	}while(option!='n' && option!='N');
	// Fin del primer "do", sale de el cuando se introduce por teclado una "n" o una "N"
	
	
	return(0);

}



int getTimeZone()
{
   TIME_ZONE_INFORMATION tziOld;
	DWORD dwRet;

	dwRet = GetTimeZoneInformation(&tziOld);

	 if(dwRet == TIME_ZONE_ID_STANDARD || dwRet == TIME_ZONE_ID_UNKNOWN)    
      tziOld.StandardBias/60;
   else if( dwRet == TIME_ZONE_ID_DAYLIGHT )
      return tziOld.DaylightBias/60;
   else
   {
      printf("GTZI failed (%d)\n", GetLastError());
      return 0;
   }

}

