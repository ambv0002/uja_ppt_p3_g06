#ifndef protocolostpte_practicas_headerfile
#define protocolostpte_practicas_headerfile
#endif

#define HELO "HELO" //identifica remitente
#define MAIL "MAIL" //comienza la transaccion de correo e identifica el remitente
#define RCPT "RCPT"  //identifica el destinatario
#define DATA "DATA" //remitente listo para transmitir
#define QUIT  "QUIT"  // Finalizacion de la sesion SMTP

//FIN DE RESPUESTA
#define CRLF "\r\n"

//ESTADOS
#define S_WELCOME 0
#define S_HELO 1
#define S_MAIL 2
#define S_RCPT 3
#define S_DATA 4
#define S_MENSAJE 5
#define S_QUIT 6

//PUERTO DEL SERVICIO
#define SMTP_SERVICE_PORT	25 //para poder conectarnos al servidor
