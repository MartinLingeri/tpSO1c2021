/*
 * fslogs.c
 *
 *  Created on: 30 jul. 2021
 *      Author: utnso
 */

#include "fslogs.h"

/*void logear(int situacion, int dato){
	break; //Para desactivar los logs si molestan para testear
}*/

void logear(int situacion, int dato){
	switch(situacion){
		case INICIO_SISTEMA:
			log_info(logger,"Iniciando Mi-RAM-HQ");
			break;

		case GENERA_OXIGENO:
			log_info(logger,"TAREA - Generando %d de oxigeno", dato);
			break;

		case CONSUME_OXIGENO:
			log_info(logger,"TAREA - Consumiendo %d de oxigeno", dato);
			break;

		case GENERA_COMIDA:
			log_info(logger,"TAREA - Generando %d de comida", dato);
			break;

		case CONSUME_COMIDA:
			log_info(logger,"TAREA - Consumiendo %d de comida", dato);
			break;

		case GENERA_BASURA:
			log_info(logger,"TAREA - Generando %d de basura", dato);
			break;

		case DESCARTA_BASURA:
			log_info(logger,"TAREA - Descartando %d de basura", dato);
			break;

		case SABOTAJE_DETECTADO:
			log_info(logger,"Sabotaje detectado en el sistema");
			break;

		case FSCK_INVOCADO:
			log_info(logger,"FSCK invocado");
			break;

		case SABOTAJE_RESUELTO:
			log_info(logger,"Sabotaje resuelto correctamente");
			break;

		case BITACORA_PEDIDA:
			log_info(logger,"%d - Bitacora pedida por el tripulante", dato);
			break;

		case BITACORA_ENVIADA:
			log_info(logger,"%d - Bitacora enviada al tripulante", dato);
			break;

		case FIN_ST:
			log_info(logger,"I-Mongo-Store finalizado");
			break;

		default:
			return;
	}

}
