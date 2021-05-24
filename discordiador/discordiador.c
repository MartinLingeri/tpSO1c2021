#include "discordiador.h"

int conexion = -1;
t_log* logger;

uint32_t id_ultima_patota = -1;
uint32_t id_ultimo_tripulante = -1;

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

	conexion = crear_conexion(config_get_string_value(config, "IP_MI_RAM_HQ"), config_get_string_value(config, "PUERTO_MI_RAM_HQ"));

	//if(conexion == -1) {
	//	puts("error en conexion");
	//	return EXIT_FAILURE;
	//}
	leer_consola(logger);
	terminar_programa(conexion, logger, config);
	sleep(30);
	return EXIT_SUCCESS;
}

t_log* iniciar_logger(void)
{
	return log_create("discordiador.log", "discordiador", true, LOG_LEVEL_INFO);
}

t_config* leer_config(void)
{
	return config_create("discordiador.config");
}

void leer_consola(t_log* logger)
{
	char* leido;
	leido = readline(">");
	while (strcmp(leido, "") != 0) {
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
	id_ultima_patota++;
	uint32_t id_patota = id_ultima_patota;
	t_buffer* buffer = serilizar_patota(id_patota, tareas);
	t_paquete* paquete_pcb = crear_pcb_mensaje(buffer);
	enviar_paquete(paquete_pcb, conexion);
	//enviar paquete pcb y esperar ok de respuesta
	pthread_t hilos[longitud];

	for(int i = 0 ; i<cantidad ; i++) {
		t_iniciar_tripulante_args* args = malloc(sizeof(t_iniciar_tripulante_args));
		args->instruccion = instruccion;
		args->cantidad_ya_iniciada = i;
		args->longitud = longitud;
		args->id_patota = id_patota;
		pthread_create(&hilos[i], NULL, inicializar_tripulante, args);
		pthread_detach((pthread_t) hilos[i]);
	}
}

void iniciar_patota_en_hq() {

}

void inicializar_tripulante(void* args){
	t_iniciar_tripulante_args* argumentos = args;
	t_tripulante* tripulante = malloc(sizeof(t_tripulante));
	tripulante->PID = argumentos->id_patota;
	char** posicion;
	if(argumentos->instruccion[3] == NULL || 3 + argumentos->cantidad_ya_iniciada >= argumentos->longitud) {
		tripulante->pos_x = 0;
		tripulante->pos_y = 0;
	} else {
		 posicion = string_split(argumentos->instruccion[3+argumentos->cantidad_ya_iniciada], "|");
		 tripulante->pos_x = atoi(posicion[0]);
		 tripulante->pos_y = atoi(posicion[1]);
	}
	tripulante->estado = e_llegada;

	//enviar paquete tcb y esperar ok de respuesta
	id_ultimo_tripulante++;
	tripulante->TID = id_ultimo_tripulante;
	list_add(llegada, tripulante);
	//pide 1er tarea
	//se le retorna la 1er tarea
	list_add(listo, tripulante);
}

