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
	MENSAJE,
}op_code;

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

t_log* logger;

void* recibir_buffer(int*, int);

int iniciar_servidor(void);
int esperar_cliente(int);
t_list* recibir_paquete(int);
void recibir_mensaje(int);
int recibir_operacion(int);
t_pcb* recibir_pcb(int socket_cliente);
t_tcb* recibir_tcb(int socket_cliente);

#endif /* CONEXIONES_H_ */
