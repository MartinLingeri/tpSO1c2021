/*
 * fscoms.c
 *
 *  Created on: 30 jul. 2021
 *      Author: utnso
 */

#include "fscoms.h"

t_buffer* serializar_sabotaje(uint32_t x, uint32_t y)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	void* stream = malloc(2*sizeof(uint32_t));

	int desplazamiento = 0;

	memcpy(stream + desplazamiento, (void*)(&x), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, (void*)(&y), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	buffer->size = desplazamiento;
	buffer->stream = stream;
	return buffer;
}

t_buffer* serializar_bitacora(char* reporte)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	void* stream = malloc(sizeof(uint32_t) + sizeof(uint32_t) + strlen(reporte) + 1);
	int desplazamiento = 0;

	uint32_t  reporte_len;
	reporte_len = strlen(reporte) + 1;
	memcpy(stream + desplazamiento, (void*)(&reporte_len), sizeof(uint32_t));
	desplazamiento += sizeof(strlen(reporte) + 1);
	memcpy(stream + desplazamiento, reporte, strlen(reporte) + 1);
	desplazamiento += strlen(reporte) + 1;

	buffer->size = desplazamiento;
	buffer->stream = stream;
	return buffer;
}

uint32_t recibir_pedir_bitacora(int socket_cliente) {
	int size;
	void* buffer;
	uint32_t id;
	int desplazamiento = 0;

	puts("BITACORA SOLICITADA");

	buffer = recibir_buffer(&size, socket_cliente);

	memcpy(&id, buffer+desplazamiento, sizeof(uint32_t));
	desplazamiento+=sizeof(uint32_t);

	printf("TRIP A BUSCAR BITACORA: %d\n", id);

	free(buffer);
	return id;
}

uint32_t recibir_invocar_fsck(int socket_cliente) {
	int size;
	void* buffer;
	uint32_t id;
	int desplazamiento = 0;

	puts("Invoca FSCK");

	buffer = recibir_buffer(&size, socket_cliente);

	memcpy(&id, buffer+desplazamiento, sizeof(uint32_t));
	desplazamiento+=sizeof(uint32_t);

	printf("TRIP QUE INVOCÓ: %d\n", id);

	free(buffer);
	return id;
}


t_hacer_tarea* recibir_hacer_tarea(int socket_cliente) {
	int size;
	void* buffer;
	int desplazamiento = 0;
	buffer = recibir_buffer(&size, socket_cliente);
	t_hacer_tarea *tarea = malloc(sizeof(t_hacer_tarea));

	puts("HACER TAREA");

	memcpy(&(tarea->cantidad), buffer+desplazamiento, sizeof(uint32_t));
	desplazamiento+=sizeof(uint32_t);

	memcpy(&(tarea->tarea), buffer+desplazamiento, sizeof(int));
	desplazamiento+=sizeof(int);

	printf("CANTIDAD: %d\n", tarea->cantidad);
	printf("NUMERO TAREA: %d\n", tarea->tarea);

	free(buffer);
	return tarea;
}


t_rlog* recibir_rbitacora(int socket_cliente) {
	int size;
	int desplazamiento = 0;
	void* buffer;
	uint32_t len;
	t_rlog *log = malloc(sizeof(t_log));

	printf("REPORTE DE BITACORA RECIBIDO\n");
	buffer = recibir_buffer(&size, socket_cliente);

	memcpy(&log->tid, buffer+desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(&(len), (buffer+desplazamiento), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	log->reporte = malloc(len);
	memcpy((log->reporte), buffer+desplazamiento, len);
	desplazamiento += len;

	printf("TRIPULANTE: %d\n", log->tid);
	printf("REPORTE: %s\n", log->reporte);

	free(buffer);
	return log;
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

void enviar_paquete(t_paquete* paquete, int socket_cliente)
{
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);
	//printf("CODIGO OP: %d\n", paquete->codigo_operacion);
	//printf("SIZE DEL BUFFER: %d\n", paquete->buffer->size);
	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

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
