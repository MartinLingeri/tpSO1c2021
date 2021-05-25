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
		puts("error");
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

t_buffer* serilizar_tripulante(uint32_t id, uint32_t pid, uint32_t pos_x, uint32_t pos_y, uint32_t estado)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	void* stream = malloc(sizeof(uint32_t) * 5);
	int desplazamiento = 0;
	memcpy(stream + desplazamiento, &id, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &pid, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &pos_x, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &pos_y, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &estado, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	buffer->size = desplazamiento;
	buffer->stream = stream;
	return buffer;
}

t_buffer* serilizar_cambio_estado(uint32_t id, uint32_t estado)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	void* stream = malloc(sizeof(uint32_t) * 2);
	int desplazamiento = 0;
	memcpy(stream + desplazamiento, &id, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &estado, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	buffer->size = desplazamiento;
	buffer->stream = stream;
	return buffer;
}

t_buffer* serilizar_pedir_tarea(uint32_t id)
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

char* recibir_tarea(int socket_cliente) {
	int size;
	int desplazamiento = 0;
	void* buffer;

	buffer = recibir_buffer(&size, socket_cliente);

	void* tarea_len = malloc(sizeof(uint32_t));
	memcpy(&tarea_len, buffer+desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	char* tarea = malloc(tarea_len);
	memcpy(tarea, buffer+desplazamiento, tarea_len);

	return tarea;
}

void mover_a(t_tripulante* tripulante, bool es_x, int valor_nuevo, int retardo_ciclo_cpu) {
      if(es_x) {
		while(tripulante->pos_x != valor_nuevo) {
			if(tripulante->pos_x < valor_nuevo) {
				tripulante->pos_x++;
			} else {
				tripulante->pos_x--;
			}
			sleep(retardo_ciclo_cpu);
		}
		printf("x: %d\n", tripulante->pos_x);
      } else {
  		while(tripulante->pos_y != valor_nuevo) {
  			if(tripulante->pos_y < valor_nuevo) {
  				tripulante->pos_y++;
  			} else {
  				tripulante->pos_y--;
  			}
  			sleep(retardo_ciclo_cpu);
  		}
  		printf("y: %d\n", tripulante->pos_y);
      }
      //registrar_movimiento(tripulante);
}
