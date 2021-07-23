/*
 * logs.c
 *
 *  Created on: 16 jul. 2021
 *      Author: utnso
 */
#include "logs.h"

/*void logear(int situacion, int dato){
	break; //Para desactivar los logs si molestan para testear
}*/

void logear(int situacion, int dato){
	switch(situacion){
		case INICIO_SISTEMA:
			log_info(logger,"Iniciando Discordiador");
			break;

		case SISTEMA_INICIADO:
			log_info(logger,"Discordiador iniciado correctamente");
			break;

		case INICIANDO_PLANIF:
			log_info(logger,"Planificación iniciada");
			break;

		case PAUSA_PLANIF:
			log_info(logger,"Planificación pausada");
			break;

		case INICIANDO_PATOTA:
			log_info(logger,"%d - Iniciando Patota", dato);
			break;

		case INICIANDO_TRIPULANTE:
			log_info(logger,"%d - Tripulante iniciado ", dato);
			break;

		case PATOTA_INICIADA:
			log_info(logger,"%d - Patota iniciada ", dato);
			break;

		case SOLICITANDO_TAREA:
			log_info(logger,"%d - Solicita próxima tarea ", dato);
			break;

	/*	case COMENZANDO_TAREA:
			log_info(logger,"%d - Comienza ejecución de tarea ", dato);
			break;*/

		/*case DESPLAZA_TAREA:
			log_info(logger,"%d - Se desplaza hacia el lugar de la tarea ", dato);
			break;*/

		case ESPERA_IO:
			log_info(logger,"%d - Ejecutando I/O ", dato);
			break;
/*
		case FINALIZA_TAREA:
			log_info(logger,"%d - Tarea finalizada ", dato);
			break;*/

		case FINALIZA_LISTA_TAREAS:
			log_info(logger,"%d - Lista de tareas completada, fin de tripulante ", dato);
			break;

		case FIN_QUANTUM:
			log_info(logger,"%d - Fin de Quantum ", dato);
			break;

		case CONTINUA_TAREA:
			log_info(logger,"%d - Continúa ejecución de tarea ", dato);
			break;

		case SABOTAJE_DETECTADO:
			log_info(logger,"Sabotaje detectado en el sistema");
			break;

	/*	case COMIENZA_ATENDER_SABOTAJE:
			log_info(logger,"%d - Comienza a atender sabotaje ", dato);
			break;*/

		case LLAMADO_FSCK:
			log_info(logger,"FSCK invocado");
			break;

		/*case SABOTAJE_ATENDIDO:
			log_info(logger,"%d - Sabotaje atendido ", dato);
			break;*/

		case BITACORA_SOLICITADA:
			log_info(logger,"%d - Bitácora solicitada ", dato);
			break;

		case ELIMINANDO_TRIPULANTE:
			log_info(logger,"%d - Expulsando tripulante ", dato);
			break;

		case TRIPULANTE_ELIMINADO:

			log_info(logger,"%d - Tripulante expulsado ", dato);
			break;

		case LISTANDO_TRIPULANTES:
			log_info(logger,"%d - Listando tripulantes por consola", dato);
			break;

		case FINALIZANDO_PROGRAMA:
			log_info(logger,"%d - Finalizando discordiador", dato);
			break;

		case PROGRAMA_FINALIZADO:
			log_info(logger,"%d - Discordiador finalizado correctamente", dato);
			break;

		default:
			return;
	}
	return;
}

void logear_error(int situacion){
	switch(situacion){

	case INST_NO_RECON:
		log_error(logger,"Instrucción no reconocida");
		break;

	case INST_FALTA_PAR:
		log_error(logger,"Algún dato de la instrucción falta o es incorrecto");
		break;

	case TRIP_NO_INICIADO:
		log_error(logger,"El tripulante no existe o no ha sido iniciado");
		break;

	case NO_MEMORIA:
		log_error(logger, "No hay lugar en memoria para guardar la patota");
		break;
	}
}

char* logs_bitacora(regs_bitacora asunto, char* dato1, char* dato2){
	int size;
	char* reporte;
	switch(asunto) {
		case B_DESPLAZAMIENTO:  //dato 1 posicion inicial en formato x|y, dato 2 posicion final en igual formato
			size = strlen(dato1) + strlen(dato2) + strlen("Se mueve de ") + strlen(" a ") + 1;
			reporte = malloc(size);
			strcpy (reporte, "Se mueve de ");
			strcat (reporte, dato1);
			strcat (reporte, " a ");
			strcat (reporte, dato2);
			return reporte;
			break;

		case INICIO_TAREA: //dato 1 nombre de tarea como string, dato 2 nada
			size = strlen(dato1) + strlen("Comienza ejecucion de la tarea ") + 1;
			reporte = malloc(size);
			strcpy (reporte, "Comienza ejecucion de la tarea ");
			strcat (reporte, dato1);
			return reporte;
			break;

		case FIN_TAREA: //dato 1 nombre de tarea, dato 2 nada
			size = strlen(dato1) + strlen("Se finaliza la tarea ") + 1;
			reporte = malloc(size);
			strcpy (reporte, "Se finaliza la tarea ");
			strcat (reporte, dato1);
			return reporte;
			break;

		case SABOTAJE:
			return "Se corre en pánico a la ubicación del sabotaje";
			break;

		case SABOTAJE_RESUELTO:
			return "Se resuelve el sabotaje";
			break;

	   default:
			return "Situación desconocida";
			break;
	}
}

void logear_despl(int pos_x, int pos_y, char* pos_x_nuevo, char* pos_y_nuevo, int id, int conexion_hq){
	int size = sizeof(int)*2 + sizeof('|');
	char *str_start = malloc(size);
	char *str_end = malloc(size);

	char *x = string_itoa(pos_x);
	char *y = string_itoa(pos_y);

	strcpy (str_start, x);
	strcat (str_start, "|");
	strcat (str_start, y);

	strcpy (str_end, pos_x_nuevo);
	strcat (str_end, "|");
	strcat (str_end, pos_x_nuevo);

	reportar_bitacora(logs_bitacora(B_DESPLAZAMIENTO, str_start, str_end), id, conexion_hq);
	free(str_start);
	free(str_end);
}

void reportar_bitacora(char* log, int id, int conexion_store){
    t_buffer* buffer = serializar_reporte_bitacora(id, log);
	t_paquete* paquete_bitacora = crear_mensaje(buffer, REPORTE_BITACORA);
	pthread_mutex_lock(&store);
	if(conexion_store != -1) {
		//printf("va a mandar al store SOCKET : %d\n", conexion_hq);
		printf("va a mandar al store xq socket store EN REPORTAR BITACORA: %d\n", conexion_store);
		enviar_paquete(paquete_bitacora, conexion_store);
	} else {
		puts("no envio a store");
	}
	pthread_mutex_unlock(&store);
	free(buffer);
	free(paquete_bitacora);
}
