#ifndef MI-RAM-HQ_H_
#define MI-RAM-HQ_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<commons/log.h>
#include "utils.h"
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
void eliminar_paquete(t_paquete* paquete);
void enviar_mensaje(char* mensaje, int socket_cliente);
void* serializar_paquete(t_paquete* paquete, int bytes);
#endif /* MI-RAM-HQ_H_ */
