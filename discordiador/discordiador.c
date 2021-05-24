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

	//conexion = crear_conexion(config_get_string_value(config, "IP_MI_RAM_HQ"), config_get_string_value(config, "PUERTO_MI_RAM_HQ"));

	//if(conexion == -1) {
	//	puts("error en conexion");
	//	return EXIT_FAILURE;
	//}
	leer_consola(logger);
	terminar_programa(conexion, logger, config);
	sleep(10);
	void printear(void* t) {
		printf("el tid es: %d\n", ((t_tripulante*) t)->TID);
	}
	list_iterate(listo, printear);
	puts("en llegada:");
	list_iterate(llegada, printear);
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
			//

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
	//t_buffer* buffer = serilizar_patota(id_patota, tareas);
	//t_paquete* paquete_pcb = crear_mensaje(buffer, PCB_MENSAJE);
	//enviar_paquete(paquete_pcb, conexion);
	//enviar paquete pcb y esperar ok de respuesta
	pthread_t hilos[longitud];

	for(int i = 0 ; i<cantidad ; i++) {
		inicializar_tripulante(instruccion, i, longitud, id_patota, hilos[i]);
	}
}

void iniciar_tripulante_en_hq(t_tripulante* tripulante) {
	t_buffer* buffer = serilizar_tripulante(tripulante->TID, tripulante->PID, tripulante->pos_x, tripulante->pos_y, tripulante->estado);
	t_paquete* paquete_tcb = crear_mensaje(buffer, TCB_MENSAJE);
	enviar_paquete(paquete_tcb, conexion);
}

void enviar_cambio_estado_hq(t_tripulante* tripulante) {
	t_buffer* buffer = serilizar_cambio_estado(tripulante->TID, tripulante->estado);
	t_paquete* paquete_cambio_estado = crear_mensaje(buffer, CAMBIO_ESTADO_MENSAJE);
	enviar_paquete(paquete_cambio_estado, conexion);
}

void inicializar_tripulante(char** instruccion, int cantidad_ya_iniciada, int longitud, int id_patota, pthread_t hilo){
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
	id_ultimo_tripulante++;
	tripulante->TID = id_ultimo_tripulante;
	list_add(llegada, tripulante);

	t_circular_args* args = malloc(sizeof(t_circular_args));
	args->tripulante = tripulante;
	pthread_create(&hilo, NULL, circular, args);
	pthread_detach((pthread_t) hilo);
}

void circular(void* args) {
	t_circular_args* argumentos = args;
	//iniciar_tripulante_en_hq(argumentos->tripulante);
	//enviar paquete tcb y esperar ok de respuesta
	//pide 1er tarea
	//se le retorna la 1er tarea
	cambiar_estado(argumentos->tripulante->estado , e_listo, argumentos->tripulante);
}

void cambiar_estado(estado estado_anterior, estado estado_nuevo, t_tripulante* tripulante){

   bool es_el_tripulante(void* tripulante_en_lista) {
		return ((t_tripulante*)tripulante_en_lista)->TID == tripulante->TID;
	}

   switch(estado_anterior){
    case e_llegada:
        list_remove_by_condition(llegada, es_el_tripulante);
        break;
    case e_listo:
        list_remove_by_condition(listo, es_el_tripulante);
        break;
    case e_fin:
        list_remove_by_condition(fin, es_el_tripulante);
        break;
    case e_trabajando:
        list_remove_by_condition(trabajando, es_el_tripulante);
        break;
    case e_bloqueado_IO:
        list_remove_by_condition(bloqueado_IO, es_el_tripulante);
        break;
    case e_bloqueado_emergencia:
        list_remove_by_condition(bloqueado_emergencia, es_el_tripulante);
        break;
   }

   tripulante->estado = estado_nuevo;

   switch(estado_nuevo){
    case e_llegada:
        list_add(llegada, tripulante);
        return;
    case e_listo:
        list_add(listo, tripulante);
        return;
    case e_fin:
        list_add(fin, tripulante);
        return;
    case e_trabajando:
        list_add(trabajando, tripulante);
        return;
    case e_bloqueado_IO:
        list_add(bloqueado_IO, tripulante);
        return;
    case e_bloqueado_emergencia:
        list_add(bloqueado_emergencia, tripulante);
        return;
    }
   //enviar_cambio_estado_hq(argumentos->tripulante);
}
