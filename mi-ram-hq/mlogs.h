/*
 * mlogs.h
 *
 *  Created on: 29 jul. 2021
 *      Author: utnso
 */

#ifndef MLOGS_H_
#define MLOGS_H_

#include"utils.h"

typedef enum
{
	INICIO_SISTEMA,
	NO_MEMORIA,
	LLEGA_TCB, //AGREGAR EN SEGM
	LLEGA_PCB, //AGREGAR EN SEGM
	A_SWAP,
	A_MEMORIA,
	SOLICITA_TAREA,
	DUMP, //AGREGAR
	FIN_HQ,
	ELIMINAR_TRIP,
}situacion;

void logear(int situacion, int dato);

#endif /* MLOGS_H_ */
