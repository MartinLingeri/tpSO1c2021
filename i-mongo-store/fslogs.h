/*
 * fslogs.h
 *
 *  Created on: 30 jul. 2021
 *      Author: utnso
 */

#ifndef FSLOGS_H_
#define FSLOGS_H_

#include"utils.h"

typedef enum
{
	INICIO_SISTEMA,
	GENERA_OXIGENO,
	CONSUME_OXIGENO,
	GENERA_COMIDA,
	CONSUME_COMIDA,
	GENERA_BASURA,
	DESCARTA_BASURA,
	SABOTAJE_DETECTADO,
	FSCK_INVOCADO,
	SABOTAJE_RESUELTO,
	BITACORA_PEDIDA,
	BITACORA_ENVIADA,
	FIN_ST,
}situacion;

void logear(int situacion, int dato);

#endif /* FSLOGS_H_ */
