#ifndef protocolostpte_practicas_headerfile
#define protocolostpte_practicas_headerfile
#endif

// COMANDOS DE APLICACION
#define SC "USER"  // SOLICITUD DE CONEXION USER usuario 
#define PW "PASS"  // Password del usuario  PASS password

#define SD  "QUIT"  // Finalizacion de la conexion de aplicacion
//#define SD2 "EXIT"  // Finalizacion de la conexion de aplicacion y finaliza servidor

#define HELO "HELO" //identifica remitente
#define MAIL "MAIL" //comienza la transaccion de correo e identifica el remitente
#define RCPT "RCPT"  //identifica el destinatario
#define DATA "DATA" //remitente listo para transmitir
//#define RSET "RSET"	//aborta la transaccion en curso y reinicia la sesion
#define QUIT  "QUIT"  // Finalizacion de la sesion SMTP
// RESPUESTAS A COMANDOS DE APLICACION
#define NOOP  "OK"  //reponde con un codigo de asentimiento positivo (250 OK)
#define ER  "ERROR"  

//FIN DE RESPUESTA
#define CRLF "\r\n"

//ESTADOS
#define S_HELO 0
#define S_MAIL 1
#define S_RCPT 2
#define S_DATA 3
#define S_MENSAJE 4
#define S_QUIT 5
#define S_EXIT 6

//PUERTO DEL SERVICIO
#define SMTP_SERVICE_PORT	25 //para poder conectarnos al servidor

// NOMBRE Y PASSWORD AUTORIZADOS
//#define USER		"alumno"
//#define PASSWORD	"123456"