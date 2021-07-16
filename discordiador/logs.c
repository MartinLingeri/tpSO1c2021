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
	int size;
	char* x = string_itoa(dato);
	char* reporte;
	switch(situacion){
		case INICIO_SISTEMA:
			size = strlen("Iniciando Discordiador") + 1;
			reporte = malloc(size);
			strcpy(reporte, "Iniciando Discordiador");
			break;

		case SISTEMA_INICIADO:
			size = strlen("Discordiador iniciado correctamente") + 1;
			reporte = malloc(size);
			strcpy(reporte, "Discordiador iniciado correctamente");
			break;

		case INICIANDO_PLANIF:
			size = strlen("Planificación iniciada") + 1;
			reporte = malloc(size);
			strcpy(reporte, "Planificación iniciada");
			break;

		case PAUSA_PLANIF:
			size = strlen("Planificación pausada") + 1;
			reporte = malloc(size);
			strcpy(reporte, "Planificación pausada");
			break;

		case INICIANDO_PATOTA:
			size = strlen(x) + strlen(" - Iniciando patota ") + 1;
			reporte = malloc(size);
			strcpy(reporte, x);
			strcat (reporte, " - Iniciando patota ");
			break;

		case INICIANDO_TRIPULANTE:
			size = strlen(x) + strlen(" - Tripulante iniciado ") + 1;
			reporte = malloc(size);
			strcpy(reporte, x);
			strcat (reporte, " - Tripulante iniciado ");
			break;

		case PATOTA_INICIADA:
			size = strlen(x) + strlen(" - Patota iniciada ") + 1;
			reporte = malloc(size);
			strcpy(reporte, x);
			strcat (reporte, " - Patota iniciada ");
			strcpy(reporte, reporte);
			break;

		case SOLICITANDO_TAREA:
			size = strlen(x) + strlen(" - Solicita próxima tarea ") + 1;
			reporte = malloc(size);
			strcpy(reporte, x);
			strcat (reporte, " - Solicita próxima tarea ");
			break;

	/*	case COMENZANDO_TAREA:
			size = strlen(x) + strlen(" - Comienza ejecución de tarea ") + 1;
			reporte = malloc(size);
			strcpy(reporte, x);
			strcat (reporte, " - Comienza ejecución de tarea ");
			break;*/

		/*case DESPLAZA_TAREA:
			size = strlen(x) + strlen(" - Se desplaza hacia el lugar de la tarea ") + 1;
			reporte = malloc(size);
			strcpy(reporte, x);
			strcat (reporte, " - Se desplaza hacia el lugar de la tarea ");
			break;*/

		case ESPERA_IO:
			size = strlen(x) + strlen(" - Ejecutando I/O ") + 1;
			reporte = malloc(size);
			strcpy(reporte, x);
			strcat (reporte, " - Ejecutando I/O ");
			break;
/*
		case FINALIZA_TAREA:
			size = strlen(x) + strlen(" - Tarea finalizada ") + 1;
			reporte = malloc(size);
			strcpy(reporte, x);
			strcat (reporte, " - Tarea finalizada ");
			break;*/

		case FINALIZA_LISTA_TAREAS:
			size = strlen(x) + strlen(" - Lista de tareas completada, fin de tripulante ") + 1;
			reporte = malloc(size);
			strcpy(reporte, x);
			strcat (reporte, " - Lista de tareas completada, fin de tripulante ");
			break;

		case FIN_QUANTUM:
			size = strlen(x) + strlen(" - Fin de Quantum ") + 1;
			reporte = malloc(size);
			strcpy(reporte, x);
			strcat (reporte, " - Fin de Quantum ");
			break;

		case CONTINUA_TAREA:
			size = strlen(x) + strlen(" - Continúa ejecución de tarea ") + 1;
			reporte = malloc(size);
			strcpy(reporte, x);
			strcat (reporte, " - Continúa ejecución de tarea ");
			break;

		case SABOTAJE_DETECTADO:
			size = strlen("Sabotaje detectado en el sistema") + 1;
			reporte = malloc(size);
			strcpy(reporte, "Sabotaje detectado en el sistema");
			break;

	/*	case COMIENZA_ATENDER_SABOTAJE:
			size = strlen(x) + strlen(" - Comienza a atender sabotaje ") + 1;
			reporte = malloc(size);
			strcpy(reporte, x);
			strcat (reporte, " - Comienza a atender sabotaje ");
			break;*/

		case LLAMADO_FSCK:
			size = strlen("Iniciando Discordiador") + 1;
			reporte = malloc(size);
			strcpy(reporte, "FSCK invocado");
			break;

		/*case SABOTAJE_ATENDIDO:
			size = strlen(x) + strlen(" - Sabotaje atendido ") + 1;
			reporte = malloc(size);
			strcpy(reporte, x);
			strcat (reporte, " - Sabotaje atendido ");
			break;*/

		case BITACORA_SOLICITADA:
			size = strlen(x) + strlen(" - Bitácora solicitada ") + 1;
			reporte = malloc(size);
			strcpy(reporte, x);
			strcat (reporte, " - Bitácora solicitada ");
			break;

		case ELIMINANDO_TRIPULANTE:
			size = strlen(x) + strlen(" - Expulsando tripulante ") + 1;
			reporte = malloc(size);
			strcpy(reporte, x);
			strcat (reporte, " - Tripulante expulsado ");
			break;

		case TRIPULANTE_ELIMINADO:
			size = strlen(x) + strlen(" - Tripulante expulsado ") + 1;
			reporte = malloc(size);
			strcpy(reporte, x);
			strcat (reporte, " - Tripulante expulsado ");
			break;

		case LISTANDO_TRIPULANTES:
			size = strlen("Listando tripulantes por consola") + 1;
			reporte = malloc(size);
			strcpy(reporte, "Listando tripulantes por consola");
			break;

		case FINALIZANDO_PROGRAMA:
			size = strlen("Finalizando discordiador") + 1;
			reporte = malloc(size);
			strcpy(reporte, "Finalizando discordiador");
			break;

		case PROGRAMA_FINALIZADO:
			size = strlen("Discordiador finalizado correctamente") + 1;
			reporte = malloc(size);
			strcpy(reporte, "Discordiador finalizado correctamente");
			break;

		case INST_NO_RECON:
			size = strlen("ERROR - Instrucción no reconocida") + 1;
			reporte = malloc(size);
			strcpy(reporte, "ERROR - Instrucción no reconocida");
			break;

		case INST_FALTA_PAR:
			size = strlen("ERROR - Algún dato de la instrucción falta o es incorrecto") + 1;
			reporte = malloc(size);
			strcpy(reporte, "ERROR - Algún dato de la instrucción falta o es incorrecto");
			break;

		case TRIP_NO_INICIADO:
			size = strlen("ERROR - El tripulante no existe o no ha sido iniciad") + 1;
			reporte = malloc(size);
			strcpy(reporte, "ERROR - El tripulante no existe o no ha sido iniciado");
			break;

		case NO_MEMORIA:
			size = strlen("ERROR - No hay lugar en memoria para guardar la patota") + 1;
			reporte = malloc(size);
			strcpy(reporte, "ERROR - No hay lugar en memoria para guardar la patota");
			break;

		default:
			return;
	}
	log_info(logger,reporte);
	free(reporte);
	return;
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
