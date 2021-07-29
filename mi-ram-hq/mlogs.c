/*
 * mlogs.c
 *
 *  Created on: 29 jul. 2021
 *      Author: utnso
 */

#include "mlogs.h"

/*void logear(int situacion, int dato){
	break; //Para desactivar los logs si molestan para testear
}*/

void logear(int situacion, int dato){
	switch(situacion){
		case INICIO_SISTEMA:
			log_info(logger,"Iniciando Mi-RAM-HQ");
			break;

		case LLEGA_TCB:
			log_info(logger,"%d - Tripulante iniciado", dato);
			break;

		case LLEGA_PCB:
			log_info(logger,"%d - Patota iniciada", dato);
			break;

		case NO_MEMORIA:
			log_error(logger, "No hay lugar en memoria");
			break;

		case A_SWAP:
			log_info(logger,"%d - Página movida a swap", dato);
			break;

		case A_MEMORIA:
			log_info(logger,"%d - Página a cargada a memoria", dato);
			break;

		case SOLICITA_TAREA:
			log_info(logger,"%d - Solicita tarea", dato);
			break;

		case DUMP:
			log_info(logger,"Señal recibida. Realizando dump de memoria");
			break;

		case FIN_HQ:
			log_info(logger,"Finalizando Mi-RAM-HQ");
			break;

		case ELIMINAR_TRIP:
			log_info(logger,"%d - Tripulante eliminado", dato);
			break;

		default:
			return;
	}

}

