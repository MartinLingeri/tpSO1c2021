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
	printf("CODIGO OP: %d\n", paquete->codigo_operacion);
	printf("SIZE DEL BUFFER: %d\n", paquete->buffer->size);
	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

int iniciar_servidor(char* ip, char* puerto)
{
	int socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(ip, puerto, &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next)
    {
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            continue;
        }
        break;
    }

	listen(socket_servidor, SOMAXCONN);
    freeaddrinfo(servinfo);

   // log_trace(logger, "Listo para escuchar a mi cliente");
    return socket_servidor;
}

int esperar_cliente(int socket_servidor)
{
	struct sockaddr_in dir_cliente;
	int tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	//log_info(logger, "Se conecto un cliente!");

	return socket_cliente;
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

t_buffer* serializar_patota(uint32_t id, char* tareas, uint32_t trips)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	void* stream = malloc(sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + strlen(tareas) + 1);

	int desplazamiento = 0;
	memcpy(stream + desplazamiento, &id, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &trips, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	void* tareas_len = malloc(sizeof(uint32_t));
	tareas_len = strlen(tareas) + 1;
	memcpy(stream + desplazamiento, (void*)(&tareas_len), sizeof(uint32_t));
	desplazamiento += sizeof(strlen(tareas) + 1);

	memcpy(stream + desplazamiento, tareas, strlen(tareas) + 1);
	desplazamiento += strlen(tareas) + 1;

	buffer->size = desplazamiento;
	buffer->stream = stream;

	printf("desplazamiento: %d\n", desplazamiento);
	printf("stream: %d\n", sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + strlen(tareas) + 1);
	return buffer;
}

t_buffer* serializar_tripulante(uint32_t id, uint32_t pid, uint32_t pos_x, uint32_t pos_y, uint32_t estado)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	void* stream = malloc(sizeof(uint32_t) * 4 + sizeof(char));
	int desplazamiento = 0;

	memcpy(stream + desplazamiento, &id, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	char e = estado_a_char(estado);

	memcpy(stream + desplazamiento, &e, sizeof(char));
	desplazamiento += sizeof(char);

	memcpy(stream + desplazamiento, &pos_x, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &pos_y, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &pid, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	buffer->size = desplazamiento;
	buffer->stream = stream;
	return buffer;
}

t_buffer* serializar_cambio_estado(uint32_t id, uint32_t estado)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	void* stream = malloc(sizeof(uint32_t) + sizeof(char));
	int desplazamiento = 0;

	memcpy(stream + desplazamiento, &id, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	char e = estado_a_char(estado);

	memcpy(stream + desplazamiento, &e, sizeof(char));
	desplazamiento += sizeof(char);

	buffer->size = desplazamiento;
	buffer->stream = stream;
	return buffer;
}

t_buffer* serializar_pedir_tarea(uint32_t id)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	void* stream = malloc(sizeof(uint32_t));
	int desplazamiento = 0;

	memcpy(stream + desplazamiento, &id, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	buffer->size = desplazamiento;
	buffer->stream = stream;
	return buffer;
}

t_buffer* serializar_reporte_bitacora(uint32_t id, char* reporte)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	void* stream = malloc(sizeof(uint32_t) + sizeof(uint32_t) + strlen(reporte) + 1);
	int desplazamiento = 0;

	memcpy(stream + desplazamiento, &id, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	void* reporte_len = malloc(sizeof(uint32_t));
	reporte_len = strlen(reporte) + 1;
	memcpy(stream + desplazamiento, (void*)(&reporte_len), sizeof(uint32_t));
	desplazamiento += sizeof(strlen(reporte) + 1);
	memcpy(stream + desplazamiento, reporte, strlen(reporte) + 1);
	desplazamiento += strlen(reporte) + 1;

	buffer->size = desplazamiento;
	buffer->stream = stream;
	printf("desplazamiento: %d\n", desplazamiento);
	printf("stream: %d\n", sizeof(uint32_t) + sizeof(uint32_t) + strlen(reporte) + 1);
	return buffer;
}

t_buffer* serializar_desplazamiento(uint32_t tid, uint32_t x_nuevo, uint32_t y_nuevo)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	void* stream = malloc(sizeof(uint32_t)*3);
	int desplazamiento = 0;

	memcpy(stream + desplazamiento, &tid, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &x_nuevo, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &y_nuevo, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	buffer->size = desplazamiento;
	buffer->stream = stream;
	return buffer;
}

t_buffer* serializar_hacer_tarea(uint32_t cantidad, int tarea, int tid)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	void* stream = malloc(sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t));
	int desplazamiento = 0;

	memcpy(stream + desplazamiento, &cantidad, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &tarea, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &tid, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	buffer->size = desplazamiento;
	buffer->stream = stream;
	return buffer;
}

t_buffer* serializar_eliminar_tripulante(uint32_t id)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	void* stream = malloc(sizeof(uint32_t));
	int desplazamiento = 0;

	memcpy(stream + desplazamiento, &id, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	buffer->size = desplazamiento;
	buffer->stream = stream;
	return buffer;
}

t_buffer* serializar_solicitar_bitacora(uint32_t id)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	void* stream = malloc(sizeof(uint32_t));
	int desplazamiento = 0;

	memcpy(stream + desplazamiento, &id, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	buffer->size = desplazamiento;
	buffer->stream = stream;
	return buffer;
}

t_buffer* invocar_fsck(uint32_t id)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	void* stream = malloc(sizeof(uint32_t));
	int desplazamiento = 0;

	memcpy(stream + desplazamiento, &id, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	buffer->size = desplazamiento;
	buffer->stream = stream;
	return buffer;
}


void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

t_sabotaje* recibir_datos_sabotaje(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_sabotaje* data = malloc(sizeof(t_sabotaje));

	buffer = recibir_buffer(&size, socket_cliente);
	memcpy(&(data->x), buffer+desplazamiento, sizeof(uint32_t));
	desplazamiento+=sizeof(uint32_t);
	memcpy(&(data->y), buffer+desplazamiento, sizeof(uint32_t));
	desplazamiento+=sizeof(uint32_t);

	free(buffer);
	return data;
	return NULL;
}

int recibir_hay_lugar(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void* buffer;

	buffer = recibir_buffer(&size, socket_cliente);

	void* lugar = malloc(sizeof(uint32_t));
	memcpy(&lugar, buffer+desplazamiento, sizeof(uint32_t));
	desplazamiento+=sizeof(uint32_t);

	free(buffer);
	return lugar;
	return NULL;
}

char* recibir_bitacora(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void* buffer;

	buffer = recibir_buffer(&size, socket_cliente);

	void* bit_len = malloc(sizeof(uint32_t));
	memcpy(&bit_len, (buffer+desplazamiento), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	char* bit = malloc(bit_len);
	memcpy(bit, buffer+desplazamiento, bit_len);
	desplazamiento += bit_len;

	free(buffer);
	return bit;
	return NULL;
}

char* recibir_tarea(int socket_cliente) {
	int size;
	int desplazamiento = 0;
	void* buffer;

	buffer = recibir_buffer(&size, socket_cliente);

	void* tarea_len = malloc(sizeof(uint32_t));
	memcpy(&tarea_len, buffer+desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	char* tarea = malloc(tarea_len);
	memcpy(tarea, buffer+desplazamiento, &tarea_len);

	return tarea;
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

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) != 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
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

void reportar_eliminar_tripulante(int id, int conexion_hq) {
    t_buffer* buffer = serializar_eliminar_tripulante(id);
	t_paquete* paquete = crear_mensaje(buffer, ELIMINAR_TRIPULANTE);
	enviar_paquete(paquete, conexion_hq);
}

void reportar_desplazamiento(int id, int nuevo_x, int nuevo_y, int conexion_hq) {
    t_buffer* buffer = serializar_desplazamiento(id, nuevo_x, nuevo_y);
	t_paquete* paquete = crear_mensaje(buffer, DESPLAZAMIENTO);
	enviar_paquete(paquete, conexion_hq);
}

