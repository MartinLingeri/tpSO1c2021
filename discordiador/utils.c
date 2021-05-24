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
		puts("error");
		return -1;
	}

	freeaddrinfo(server_info);

	return socket_cliente;
}

void enviar_mensaje(char* mensaje, int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}


void crear_buffer(t_paquete* paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

t_paquete* crear_tcb_mensaje(t_buffer* buffer)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = TCB_MENSAJE;
	paquete->buffer = buffer;
	return paquete;
}

t_paquete* crear_pcb_mensaje(t_buffer* buffer)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = PCB_MENSAJE;
	paquete->buffer = buffer;
	return paquete;
}

void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);
	paquete->buffer->size += tamanio + sizeof(int);
}

t_buffer* serilizar_patota(uint32_t id, char* tareas)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	void* stream = malloc(sizeof(uint32_t) + sizeof(uint32_t) + strlen(tareas) + 1);
	int desplazamiento = 0;
	memcpy(stream + desplazamiento, &id, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	void* tareas_len = malloc(sizeof(uint32_t));
	tareas_len = strlen(tareas) + 1;
	memcpy(stream + desplazamiento, (void*)(&tareas_len), sizeof(uint32_t));
	desplazamiento += sizeof(strlen(tareas) + 1);
	memcpy(stream + desplazamiento, tareas, strlen(tareas) + 1);
	desplazamiento += strlen(tareas) + 1;
	buffer->size = desplazamiento;
	buffer->stream = stream;
	return buffer;
}

void enviar_paquete(t_paquete* paquete, int socket_cliente)
{
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
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

void recibir_mensaje(int socket_cliente, t_log* logger)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "DISCORDIADIADOR Me llego el mensaje %s", buffer);
	free(buffer);
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

void* leer_tareas(t_tripulante* tripulante, char* tarea){

    char** parametros_tarea = string_split(tarea, ";");
	char** nombre_tarea = string_split(parametros_tarea[0], " ");
	char* pos_x = tarea[1];
	char* pos_y = tarea[2];
	char* duracion = tarea[3];

	mover_a(tripulante,'x',pos_x);
	mover_a(tripulante,'y',pos_y);
	//como controlar quantum en caso de RR sleep(atoi(duracion));

	if(strcmp(nombre_tarea[0], "GENERAR_OXIGENO") == 0) {
		//generar_oxigeno(nombre_tarea[1]);
	} else if (strcmp(nombre_tarea[0], "CONSUMIR_OXIGENO") == 0) {
		//consumir_oxigeno(nombre_tarea[1]);
	} else if (strcmp(nombre_tarea[0], "GENERAR_COMIDA") == 0) {
		//generar_comida(nombre_tarea[1]);
	} else if (strcmp(nombre_tarea[0], "CONSUMIR_COMIDA") == 0) {
		//consumir_comida(nombre_tarea[1]);
	} else if (strcmp(nombre_tarea[0], "GENERAR_BASURA") == 0) {
		//generar_basura(nombre_tarea[1]);
	} else if (strcmp(nombre_tarea[0], "DESCARTAR_BASURA") == 0) {
		//destruir_basura();
	} else {
		//log_info(logger, "no se reconocio la tarea");
	}
	//actualizar_bitacora(tripulante);
	return 0;
}

void mover_a(t_tripulante* tripulante, char xOy, char valor_nuevo){
      if(xOy == 'x'){
            tripulante->pos_x = valor_nuevo;
      }else{
            tripulante->pos_y = valor_nuevo;
      }
      //registrar_movimiento(tripulante);
}
