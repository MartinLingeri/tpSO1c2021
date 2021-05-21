#include "discordiador.h"

int conexion = -1;
t_log* logger;

t_list* llegada;
t_list* listo;
t_list* fin;
t_list* trabajando;
t_list* bloqueado_IO;
t_list* bloqueado_emergencia;

int main(void)
{
	llegada = list_create();
	listo = list_create();
	fin = list_create();
	trabajando = list_create();
	bloqueado_IO = list_create();
	bloqueado_emergencia = list_create();

	logger = iniciar_logger();
	t_config* config = leer_config();

	//conexion = crear_conexion(config_get_string_value(config, "IP_MI_RAM_HQ"), config_get_string_value(config, "PUERTO_MI_RAM_HQ"));

	//if(conexion == -1) {
	//	puts("error en conexion");
	//	return EXIT_FAILURE;
	//}
	leer_consola(logger);
	terminar_programa(conexion, logger, config);

	return EXIT_SUCCESS;
}

t_log* iniciar_logger(void)
{
	return log_create("discordiador.log", "discordiador", true, LOG_LEVEL_INFO);
}

t_config* leer_config(void)
{
	config_create("discordiador.config");
}

void leer_consola(t_log* logger)
{
	char* leido;
	leido = readline(">");
	while (strcmp(leido, "") != 0) {
		int a = getppid();
		int aa = getpid();

		char** instruccion = string_split(leido, " ");

		if(strcmp(instruccion[0], "INICIAR_PATOTA") == 0) {
			iniciar_patota(instruccion, leido);

		} else if (strcmp(instruccion[0], "LISTAR_TRIPULANTES") == 0) {

		} else if (strcmp(instruccion[0], "EXPULSAR_TRIPULANTE") == 0) {

		} else if (strcmp(instruccion[0], "INICIAR_PLANIFICACION") == 0) {

		} else if (strcmp(instruccion[0], "PAUSAR_PLANIFICACION") == 0) {

		} else if (strcmp(instruccion[0], "OBTENER_BITACORA") == 0) {

		} else {
			log_info(logger, "no se reconocio la instruccion");
		}

		leido = readline(">");
	}
	free(leido);
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	liberar_conexion(conexion);
	log_destroy(logger);
	config_destroy(config);
}

int longitud_instr(char** instruccion){
	int largo = 0;
	int i = 0;
	while(instruccion[i] != NULL){
		largo++;
		i++;
	}
	return largo;
}

void iniciar_patota(char** instruccion, char* leido) {
	int cantidad = atoi(instruccion[1]);
	char* tareas = instruccion[2];
	int longitud = longitud_instr(instruccion);

	t_paquete* paquete_pcb = crear_pcb_mensaje();
	agregar_a_paquete(paquete_pcb, tareas,  strlen(tareas) + 1);
	//enviar paquete pcb y esperar pid de respuesta
	//int id_patota = la respuesta de enviar la patota
	int id_patota = 0;

	for(int i = 0; i < cantidad; i++) {
		inicializar_tripulante(instruccion, i, longitud, id_patota);
	}
}

void iniciar_patota_en_hq() {

}

t_tripulante* inicializar_tripulante(char** instruccion, int cantidad_ya_iniciada, int longitud, int id_patota){
	t_tripulante* tripulante = malloc(sizeof(t_tripulante));
	tripulante->PID = id_patota;
	char** posicion;
	if(instruccion[3] == NULL || 3 + cantidad_ya_iniciada >= longitud) {
		tripulante->pos_x = 0;
		tripulante->pos_y = 0;
	} else {
		 posicion = string_split(instruccion[3+cantidad_ya_iniciada], "|");
		 tripulante->pos_x = atoi(posicion[0]);
		 tripulante->pos_y = atoi(posicion[1]);
	}
	tripulante->estado = e_llegada;

	t_paquete* paquete_tcb = crear_tcb_mensaje();
	agregar_a_paquete(paquete_tcb, tripulante->PID, sizeof(int));
	agregar_a_paquete(paquete_tcb, tripulante->pos_x, sizeof(int));
	agregar_a_paquete(paquete_tcb, tripulante->pos_y, sizeof(int));
	agregar_a_paquete(paquete_tcb, (void*)tripulante->estado, sizeof(int));
	//enviar paquete tcb y esperar tid de respuesta
	//tripulante->TID = la respuesta del envio del tripulante
	list_add(llegada, tripulante);
	//pide 1er tarea
	//se le retorna la 1er tarea
	list_add(listo, tripulante);
	return tripulante;
}

