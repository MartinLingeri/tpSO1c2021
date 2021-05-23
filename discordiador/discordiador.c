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
	config_create("discordiador.config");
}

void leer_consola(t_log* logger)
{
	char* leido = malloc(sizeof( "INICIAR_PATOTA 5 /home/utnso/tareas/tareasPatota5.txt 1|1 3|4"));
	leido = "INICIAR_PATOTA 5 /home/utnso/tareas/tareasPatota5.txt 1|1 3|4";
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

		//leido = readline(">");

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
	uint32_t id = 1;
	puts("1");
	t_buffer* buffer = serilizar_patota(id, tareas);
	t_paquete* paquete_pcb = crear_pcb_mensaje(buffer);
	enviar_paquete(paquete_pcb, conexion);
	//enviar paquete pcb y esperar pid de respuesta
	//int id_patota = la respuesta de enviar la patota
	int id_patota = 0;
	pthread_t hilos[longitud];

	/*for(int i = 0 ; i<cantidad ; i++) {
		t_iniciar_tripulante_args* args = malloc(sizeof(t_iniciar_tripulante_args));
		args->instruccion = instruccion;
		args->cantidad_ya_iniciada = i;
		args->longitud = longitud;
		args->id_patota = id_patota;
		pthread_create(&hilos[i], NULL, inicializar_tripulante, args);
		pthread_detach((pthread_t) hilos[i]);
	}*/
}

void iniciar_patota_en_hq() {

}

void inicializar_tripulante(void* args){
	t_iniciar_tripulante_args* argumentos = args;
	printf("cant ya iniciada: %d\n", argumentos->cantidad_ya_iniciada);
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
	printf("pos x: %d.......pox y: %d\n",  tripulante->pos_x,  tripulante->pos_y);
	tripulante->estado = e_llegada;

	//enviar paquete tcb y esperar tid de respuesta
	//tripulante->TID = la respuesta del envio del tripulante
	list_add(llegada, tripulante);
	//pide 1er tarea
	//se le retorna la 1er tarea
	list_add(listo, tripulante);
	//return tripulante;
}

