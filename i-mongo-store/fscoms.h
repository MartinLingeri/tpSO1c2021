/*
 * fscoms.h
 *
 *  Created on: 30 jul. 2021
 *      Author: utnso
 */

#ifndef FSCOMS_H_
#define FSCOMS_H_

#include "utils.h"

typedef struct{
	uint32_t cantidad;
	int tarea;
}t_hacer_tarea;

typedef struct{
	uint32_t tid;
	char* reporte;
}t_rlog;

t_buffer* serializar_bitacora(char* tarea);
t_buffer* serializar_sabotaje(uint32_t x, uint32_t y);
uint32_t recibir_pedir_bitacora(int socket_cliente);
t_hacer_tarea* recibir_hacer_tarea(int socket_cliente);
uint32_t recibir_invocar_fsck(int socket_cliente);
t_rlog* recibir_rbitacora(int socket_cliente);
int crear_conexion(char *ip, char* puerto);
t_paquete* crear_mensaje(t_buffer* buffer, op_code codigo);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void* serializar_paquete(t_paquete* paquete, int bytes);

#endif /* FSCOMS_H_ */
