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

typedef enum
{
	PCB_MENSAJE,
	TCB_MENSAJE,
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

int crear_conexion(char* ip, char* puerto);
void enviar_mensaje(char* mensaje, int socket_cliente);
t_paquete* crear_pcb_mensaje(t_buffer* buffer);
t_paquete* crear_tcb_mensaje(t_buffer* buffer);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void liberar_conexion(int socket_cliente);
void eliminar_paquete(t_paquete* paquete);
void* recibir_buffer(int*, int);
void recibir_mensaje(int, t_log*);
int recibir_operacion(int socket_cliente);

#endif /* UTILS_H_ */
