#ifndef CONEXIONES_H_
#define CONEXIONES_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/config.h>
#include<commons/collections/list.h>
#include<string.h>
#include<semaphore.h>
#include<stdbool.h>
#include<pthread.h>
#include<sys/mman.h>
#include<unistd.h>
#include<fcntl.h>
#include<signal.h>

extern int conexion;

typedef struct{
	uint32_t size;
	void* stream;
} t_buffer;

typedef struct{
	uint8_t codigo_operacion;
	t_buffer* buffer;
} t_paquete;

typedef enum
{
	PCB_MENSAJE,
	TCB_MENSAJE,
	CAMBIO_ESTADO_MENSAJE,
	PEDIR_SIGUIENTE_TAREA,
	MENSAJE,
    REPORTE_BITACORA,
    DESPLAZAMIENTO,
    HACER_TAREA,
	ELIMINAR_TRIPULANTE,
	PEDIR_BITACORA,
	INVOCAR_FSCK,
}op_code;

typedef enum
{
	PCB,
	TAREAS,
	TCB,
}tipo_contenido;

typedef struct{
	uint32_t pid;
	char* tareas;
}t_pcb;

typedef struct{
	uint32_t tid;
	char estado;
	uint32_t pos_x;
	uint32_t pos_y;
	uint32_t proxima_instruccion;
	uint32_t pcb;
}t_tcb;

typedef struct{
	uint32_t tid;
	uint32_t pos_x;
	uint32_t pos_y;
}t_despl;

typedef struct{
	uint32_t pid;
	uint32_t cantTripulantes;
	char *tareas;
}t_iniciar_patota;

void *inicio_memoria;

pthread_mutex_t cargar;

t_log* logger;
t_config* config;

t_config* leer_config(void);
t_log* iniciar_logger(void);

//-------------------------

void* recibir_buffer(int*, int);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
t_paquete* crear_mensaje(t_buffer* buffer, op_code codigo);
int iniciar_servidor(void);
int esperar_cliente(int);
t_list* recibir_paquete(int);
void recibir_mensaje(int);
int recibir_operacion(int);
t_iniciar_patota* recibir_pcb(int socket_cliente);
t_tcb* recibir_tcb(int socket_cliente);
uint32_t recibir_pedir_tarea(int socket_cliente);
t_tcb* recibir_cambio_estado(int socket_cliente);
t_tcb* recibir_desplazamiento(int socket_cliente);
uint32_t recibir_eliminar_tripulante(int socket_cliente);
t_buffer* serializar_hay_lugar_memoria(uint32_t id);
t_buffer* serializar_tarea(uint32_t id, char* tarea);
#endif /* CONEXIONES_H_ */
