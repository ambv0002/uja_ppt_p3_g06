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

#include "protocol.h"





int main(int *argc, char *argv[])
{
	SOCKET sockfd; // Definimos un socket, lo llamamos sockfd
	struct sockaddr_in server_in; // Estructura basica para llamadas al sistema y funciones relacionada con direcciones de Internet
	char buffer_in[1024], buffer_out[1048],origen[15],destino[15],mensaje[500]="",asunto[20],copia[500]; // Buffers y entrada de datos
	int recibidos=0,enviados=0; // Variables para el envio y recepcion de los datos, ambas iniciadas a 0
	int estado=S_HELO; // Estados de la conexion iniciado a S_HELO
	char option; // Variable condicional para el bucle "do"
	WORD wVersionRequested; // Para trabajar con sockets en Windows
	WSADATA wsaData; // Para trabajar con sockets en Windows
	int err,fin=0,opcion; // Usamos "err" para la comprobacion de errores
    char ipdest[16];// Declaramos la direccion IP destino
	char default_ip[16]="127.0.0.1"; // Direccion IP establecida por defecto
	int i;
	/*Para la fecha y hora*/
	time_t tiempo = time(0);
	struct tm *tlocal = localtime(&tiempo);
	char zona[10];
	char output[20];

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
			
			estado=S_WELCOME; // Se reestablece el estado inicial
		
			// establece la conexion de transporte
			if(connect(sockfd,(struct sockaddr*)&server_in,sizeof(server_in))==0)
			{
				printf("CLIENTE> CONEXION ESTABLECIDA CON %s:%d\r\n",ipdest,SMTP_SERVICE_PORT);
				fin=0;
			
				//Inicio de la máquina de estados

				do{
					fflush(stdin); /*limpiamos el buffer de teclado*/
						if(estado==S_QUIT  && strncmp(buffer_in,"2",1)==0)
						{
							char opcion;

							printf("¿Deseas enviar otro correo? S/N\n");
							opcion=getch();
							if(opcion=='s' || opcion=='S')
							{
							estado=S_MAIL; /*Pasamos al estado de destinatario*/
							}
						 }
				
					switch(estado)
					{

					case S_WELCOME:
						// Se recibe el mensaje de bienvenida
						
						break;
					case S_HELO:
						// Se prepara el comando helo
						sprintf_s (buffer_out, sizeof(buffer_out), "HELO %s%s",ipdest,CRLF);
						break;

					case S_MAIL:
						// establece la conexion de aplicacion 
						
						printf("CLIENTE> Introduzca su usuario: ");
						gets(origen);
						fflush(stdin);
						if(strlen(origen)==0)
						{
							do{
								printf("CLIENTE> Introduzca su usuario: ");
								gets(origen);
							}while(strlen(origen)==0);
						sprintf_s (buffer_out, sizeof(buffer_out), "MAIL FROM: %s%s",origen,CRLF);
						}
						else
						{
						/*Envia y recibe los datos asociados con el destinatario*/
						sprintf_s (buffer_out, sizeof(buffer_out), "MAIL FROM: %s%s",origen,CRLF);

						}
						break;

					case S_RCPT:
						
						printf("CLIENTE> Introduzca el destinatario: ");
						gets(destino);
						if(strlen(destino)==0)
						{
							do{
								printf("CLIENTE> Introduzca el destinatario: ");
								gets(destino);
								
							}while(strlen(destino)==0);
						sprintf_s (buffer_out, sizeof(buffer_out), "RCPT TO: %s%s",destino,CRLF);
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
					
					 strftime(output,128,"%d/%m/%y %H:%M:%S",tlocal);				
					 printf("Introduce el asunto del mensaje:");
					 gets(asunto);	
					 
					 sprintf_s(buffer_out, sizeof(buffer_out), "Date: %s GMT:%d%sFrom: <%s>%sSubject: %s%sTo: <%s>%s",output,getTimeZone(),CRLF,origen,CRLF,asunto,CRLF,destino,CRLF);

					 enviados=send(sockfd,buffer_out,(int)strlen(buffer_out),0);
					 if(enviados==SOCKET_ERROR || enviados==0)
						{
							if(enviados==SOCKET_ERROR)
							{
							DWORD error=GetLastError();
							printf("CLIENTE> Error %d en el envío de datos\r\n",error);
							fin=1;
							}
							else
							{
							printf("CLIENTE> Conexión con el servidor cerrada\r\n");
							fin=1;
							}
							continue;
						}
									 
					 printf("Introduce el mensaje: ");
						do
						{
							gets(copia);
							sprintf_s(buffer_out, sizeof(buffer_out), "%s%s%s\0",CRLF,copia,CRLF);
						/*	for(i=0; i<strlen(copia); i++)
								buffer_out[i] = '\0'; */
							if(copia!=".")
							{
							fflush(stdin);
							enviados=send(sockfd,buffer_out,sizeof(buffer_out),0);
								if(enviados==SOCKET_ERROR || enviados==0)
								{
									if(enviados==SOCKET_ERROR)
									{
										DWORD error=GetLastError();
										printf("CLIENTE> Error %d en el envío de datos\r\n",error);
										fin=1;
									}
									else
									{
										printf("CLIENTE> Conexión con el servidor cerrada\r\n");
										fin=1;
									}
								continue;
								}
							}
						}while(strcmp(copia,".")!=0);
										
						sprintf_s(buffer_out, sizeof(buffer_out), ".%s",CRLF);
						strcat(buffer_out,"\0");
					break;

				case S_QUIT:
					sprintf_s (buffer_out, sizeof(buffer_out), "QUIT%s",CRLF);
					fin=1;
					break;
					}/*Cierre del switch*/
								
				
					if(estado!=S_WELCOME){
						enviados=send(sockfd,buffer_out,(int)strlen(buffer_out),0);
						if(enviados==SOCKET_ERROR || enviados==0)
						{
							if(enviados==SOCKET_ERROR)
							{
							DWORD error=GetLastError();
							printf("CLIENTE> Error %d en el envío de datos\r\n",error);
							fin=1;
							}
							else
							{
							printf("CLIENTE> Conexión con el servidor cerrada\r\n");
							fin=1;
							}
							continue;
						}
					}
						
					
					recibidos=recv(sockfd,buffer_in,512,0);
					fflush(stdin);
					
					if(recibidos<=0) // Si recibimos 0 o -1
					{
						DWORD error=GetLastError();
						if(recibidos<0) // Si recibimos un -1 o SOCKET_ERROR la operacion ha fallado
						{
							printf("CLIENTE> Error %d en la recepción de datos\r\n",error);  // Salimos con "estado=S_QUIT"
							fin=1;
						}
						else // Si recibimos un 0, la conexion ha sido liberada de forma acordada
						{
							printf("CLIENTE> Conexión con el servidor cerrada\r\n");
							fin=1;
						
					
						}
					}else{  // Si recibimos la cantidad de bytes enviados
						
						buffer_in[recibidos]=0x00; // Iniciamos a 0 porque en C los arrays finalizan con el byte 0000 0000
						printf(buffer_in); // Imprimimos por pantalla el valor
 
						if((estado==S_WELCOME || estado==S_HELO || estado==S_MAIL || estado==S_RCPT || estado==S_MENSAJE) && strncmp(buffer_in,"2",1)==0)
							estado++;
						
						if(estado==S_DATA && strncmp(buffer_in,"3",1)==0)
							estado++;

						
					}

				}while(fin!=1); 
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
				
