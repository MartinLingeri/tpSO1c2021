#ifndef UTILS_H_
#define UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>
#include <semaphore.h>

typedef enum
{
	PCB_MENSAJE,
	TCB_MENSAJE,
	CAMBIO_ESTADO_MENSAJE,
	PEDIR_SIGUIENTE_TAREA,
	MENSAJE,
}op_code;

typedef enum
{
	e_llegada,
	e_listo,
	e_fin,
	e_trabajando,
	e_bloqueado_IO,
	e_bloqueado_emergencia,
}estado;

typedef struct {
int TID;
int PID;
int pos_x;
int pos_y;
estado estado;
sem_t semaforo;
}t_tripulante;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

void* serializar_paquete(t_paquete* paquete, int bytes);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
int crear_conexion(char* ip, char* puerto);
t_paquete* crear_mensaje(t_buffer* buffer, op_code codigo);
t_buffer* serilizar_patota(uint32_t id, char* tareas);
t_buffer* serilizar_tripulante(uint32_t id, uint32_t pid, uint32_t pos_x, uint32_t pos_y, uint32_t estado);
t_buffer* serilizar_cambio_estado(uint32_t id, uint32_t estado);
t_buffer* serilizar_pedir_tarea(uint32_t id);
void eliminar_paquete(t_paquete* paquete);
void liberar_conexion(int socket_cliente);
void* recibir_buffer(int* size, int socket_cliente);
int recibir_operacion(int socket_cliente);
char* recibir_tarea(int socket_cliente);
void* leer_tareas(t_tripulante* tripulante, char* tarea);
void mover_a(t_tripulante* tripulante, char xOy, char valor_nuevo);

#endif /* UTILS_H_ */
