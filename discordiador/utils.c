#include "utils.h"

void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void * buffer_serializado = malloc(bytes);
	int desplazamiento = 0;

	memcpy(buffer_serializado + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(buffer_serializado + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(buffer_serializado + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return buffer_serializado;
}

void enviar_paquete(t_paquete* paquete, int socket_cliente)
{
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);
	//printf("CODIGO OP: %d\n", paquete->codigo_operacion);
	//printf("SIZE DEL BUFFER: %d\n", paquete->buffer->size);
	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1) {
		return -1;
	}

	freeaddrinfo(server_info);
	return socket_cliente;
}

t_paquete* crear_mensaje(t_buffer* buffer, op_code codigo)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = codigo;
	paquete->buffer = buffer;
	return paquete;
}

void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void mover_a(t_tripulante* tripulante, bool es_x, int valor_nuevo, int retardo_ciclo_cpu) {
      if(es_x) {
			if(tripulante->pos_x < valor_nuevo) {
				tripulante->pos_x++;
			} else {
				tripulante->pos_x--;
			}
			sleep(retardo_ciclo_cpu);

      } else {
  			if(tripulante->pos_y < valor_nuevo) {
  				tripulante->pos_y++;
  			} else {
  				tripulante->pos_y--;
  			}
  			sleep(retardo_ciclo_cpu);
      }
}

char* logs_bitacora(regs_bitacora asunto, char* dato1, char* dato2){
	int size;
	char* reporte;
	switch(asunto) {
		case B_DESPLAZAMIENTO:  //dato 1 posicion inicial en formato x|y, dato 2 posicion final en igual formato
			size = strlen(dato1) + strlen(dato2) + strlen("Se mueve de ") + strlen(" a ") + 1;
			reporte = malloc(size);
			strcpy (reporte, "Se mueve de ");
			strcat (reporte, dato1);
			strcat (reporte, " a ");
			strcat (reporte, dato2);
			return reporte;
			break;

		case INICIO_TAREA: //dato 1 nombre de tarea como string, dato 2 nada
			size = strlen(dato1) + strlen("Comienza ejecucion de la tarea ") + 1;
			reporte = malloc(size);
			strcpy (reporte, "Comienza ejecucion de la tarea ");
			strcat (reporte, dato1);
			return reporte;
			break;

		case FIN_TAREA: //dato 1 nombre de tarea, dato 2 nada
			size = strlen(dato1) + strlen("Se finaliza la tarea ") + 1;
			reporte = malloc(size);
			strcpy (reporte, "Se finaliza la tarea ");
			strcat (reporte, dato1);
			return reporte;
			break;

		case SABOTAJE:
			return "Se corre en pánico a la ubicación del sabotaje";
			break;

		case SABOTAJE_RESUELTO:
			return "Se resuelve el sabotaje";
			break;

	   default:
			return "Situación desconocida";
			break;
	}
}

char estado_a_char(int estado){
   switch(estado){
    case e_llegada:
        return 'N';
        break;
    case e_listo:
        return 'R';
        break;
    case e_fin:
        return 'E';
        break;
    case e_trabajando:
        return 'W';
        break;
    case e_bloqueado_IO:
        return 'B';
        break;
    case e_bloqueado_emergencia:
        return 'B';
        break;
    default:
    	return '?';
    	break;
   }
}

int atoi_tarea(char* tarea){
	if(strcmp(tarea, "GENERAR_OXIGENO") == 0) {
		return GENERAR_OXIGENO;
	} else if (strcmp(tarea, "CONSUMIR_OXIGENO") == 0) {
		return CONSUMIR_OXIGENO;
	} else if (strcmp(tarea, "GENERAR_COMIDA") == 0) {
		return GENERAR_COMIDA;
	} else if (strcmp(tarea, "CONSUMIR_COMIDA") == 0) {
		return CONSUMIR_COMIDA;
	} else if (strcmp(tarea, "GENERAR_BASURA") == 0) {
		return GENERAR_BASURA;
	} else if (strcmp(tarea, "DESCARTAR_BASURA") == 0) {
		return DESCARTAR_BASURA;
	} else {
		return -1;
	}
}

int longitud_instr(char** instruccion) {
	int largo = 0;
	int i = 0;
	while(instruccion[i] != NULL){
		largo++;
		i++;
	}
	return largo;
}

double distancia(t_tripulante* trip, int x, int y){
    double valor = ((x - trip->pos_x)*(x - trip->pos_x) + (y - trip->pos_y)*(y - trip->pos_y));
    return sqrt(valor);
}
