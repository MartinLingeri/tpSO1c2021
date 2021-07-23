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
	uint32_t idPatota;
	void *paginas;
}tabla_de_paginas;

typedef struct{
	uint32_t nroPagina;
	void *frame;
}pagina;

typedef struct{
	bool libre;
	t_pcb *pcb;
	char *tareas;
	t_tcb *tcb;
}frame;


t_list listaDeTablasDePaginas;
t_list listaDeFrames;
void *puntero_memoria_principal;

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
t_pcb* recibir_pcb(int socket_cliente);
t_tcb* recibir_tcb(int socket_cliente);
uint32_t recibir_pedir_tarea(int socket_cliente);
void recibir_cambio_estado(int socket_cliente);
void recibir_desplazamiento(int socket_cliente);
void recibir_eliminar_tripulante(int socket_cliente);
//DE ACA PARA ABAJO SON DESERIALIZACIONES DE I-MONGO-STORE
void recibir_pedir_bitacora(int socket_cliente);
void recibir_hacer_tarea(int socket_cliente);
void recibir_invocar_fsck(int socket_cliente);
void recibir_rbitacora(int socket_cliente);
#endif /* CONEXIONES_H_ */
