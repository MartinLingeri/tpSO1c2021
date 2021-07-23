#ifndef LOGS_H_
#define LOGS_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<commons/string.h>
#include<sys/socket.h>
#include<netdb.h>
#include<pthread.h>
#include<string.h>
#include<commons/log.h>
#include<semaphore.h>
#include<commons/collections/list.h>
#include<math.h>

#include "utils.h"
#include "serializaciones.h"

typedef enum
{
	INICIO_SISTEMA,
	SISTEMA_INICIADO,
	INICIANDO_PLANIF,
	PAUSA_PLANIF,
	INICIANDO_PATOTA,
	INICIANDO_TRIPULANTE,
	PATOTA_INICIADA,
	SOLICITANDO_TAREA,
	COMENZANDO_TAREA,
	DESPLAZA_TAREA,
	ESPERA_IO,
	FINALIZA_TAREA,
	FINALIZA_LISTA_TAREAS,
	FIN_QUANTUM,
	CONTINUA_TAREA,
	SABOTAJE_DETECTADO,
	COMIENZA_ATENDER_SABOTAJE,
	LLAMADO_FSCK,
	SABOTAJE_ATENDIDO,
	BITACORA_SOLICITADA,
	ELIMINANDO_TRIPULANTE,
	TRIPULANTE_ELIMINADO,
	LISTANDO_TRIPULANTES,
	FINALIZANDO_PROGRAMA,
	PROGRAMA_FINALIZADO,
	INST_NO_RECON,
	INST_FALTA_PAR,
	TRIP_NO_INICIADO,
	NO_MEMORIA,
}situacion;

void logear(int situacion, int dato);
void logear_error(int situacion);
char* logs_bitacora(regs_bitacora asunto, char* dato1, char* dato2);
void reportar_bitacora(char* log, int id, int conexion_store);
void logear_despl(int pos_x, int pos_y, char* pos_x_nuevo, char* pos_y_nuevo, int id, int conexion_hq);

#endif /* LOGS_H_ */
