#include "utils.h"

t_buffer* serializar_patota(uint32_t id, char* tareas, uint32_t trips)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	void* stream = malloc(sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + strlen(tareas) + 1);

	int desplazamiento = 0;
	memcpy(stream + desplazamiento, &id, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &trips, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	uint32_t tareas_len;
	tareas_len = strlen(tareas) + 1;
	memcpy(stream + desplazamiento, (void*)(&tareas_len), sizeof(uint32_t));
	desplazamiento += sizeof(strlen(tareas) + 1);

	memcpy(stream + desplazamiento, tareas, strlen(tareas) + 1);
	desplazamiento += strlen(tareas) + 1;

	buffer->size = desplazamiento;
	buffer->stream = stream;
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

	uint32_t  reporte_len;
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

t_buffer* serializar_hacer_tarea(uint32_t cantidad, int tarea)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	void* stream = malloc(sizeof(uint32_t) + sizeof(uint32_t));
	int desplazamiento = 0;

	memcpy(stream + desplazamiento, &cantidad, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &tarea, sizeof(uint32_t));
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

t_sabotaje* recibir_datos_sabotaje(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void* buffer;
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

uint32_t recibir_hay_lugar(int socket_cliente)
{
	void* nrecibir_buffer(int* size, int socket_cliente)
	{
		void* buffer;

		recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
		buffer = malloc(*size);
		printf("BUFFER SIZE: %d\n", sizeof(buffer));
		recv(socket_cliente, buffer, sizeof(buffer), MSG_WAITALL);

		return buffer;
	}

	int size;
	int desplazamiento = 0;
	void* buffer;

	buffer = nrecibir_buffer(&size, socket_cliente);
	puts("buffer rcv");

	void* dato = malloc(sizeof(uint32_t));
	printf("ESPACIO PARA ALOJAR: %d\n", sizeof(dato));

	memcpy(&(dato), (buffer+desplazamiento), sizeof(uint32_t));

	printf("VALOR RECIBIDO: %d\n", (dato));

	desplazamiento += sizeof(uint32_t);
	puts("dps memcpy y desplazar");
	free(buffer);
	return 1;
}

char* recibir_bitacora(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void* buffer;

	buffer = recibir_buffer(&size, socket_cliente);

	uint32_t bit_len;
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

	uint32_t tarea_len;
	memcpy(&tarea_len, buffer+desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	char* tarea = malloc(tarea_len);
	memcpy(tarea, buffer+desplazamiento, tarea_len);

	return tarea;
}
