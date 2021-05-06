#include "discordiador.h"

int id_ultimo_tripulante = 0;
int id_ultima_patota = 0;

int main(void)
{
	int conexion;
	/*char* ip;
	char* puerto;
	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");
	puts(puerto);
	conexion = crear_conexion(ip, puerto);
	if(conexion == -1) {
		log_error(logger, "error en la conexion");
		return EXIT_FAILURE;
	}
	enviar_mensaje(valor, conexion);
	paquete(conexion);*/
	t_log* logger;
	t_config* config;
	logger = iniciar_logger();
	config = leer_config();
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
		log_info(logger, leido);

		char** instruccion = string_split(leido, " ");

		if(strcmp(instruccion[0], "INICIAR_PATOTA") == 0) {
			iniciar_patota(instruccion);

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

void paquete(int conexion)
{
	char* leido;
	t_paquete* paquete = crear_paquete();
	leido = readline(">");
	while (strcmp(leido, "") != 0) {
		agregar_a_paquete(paquete, leido, strlen(leido) + 1);

		leido = readline(">");
	}

	enviar_paquete(paquete, conexion);

	free(leido);
	eliminar_paquete(paquete);
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

void iniciar_patota(char** instruccion) {
	int cantidad = atoi(instruccion[1]);
	char* tareas = instruccion[2];
	int longitud = longitud_instr(instruccion);
	int id_patota = id_ultima_patota++;
	id_ultima_patota++;
	for(int i = 0; i < cantidad; i++) {
		inicializar_tripulante(instruccion, i, longitud, id_patota);
	}
}

void iniciar_patota_en_hq() {

}

t_tripulante* inicializar_tripulante(char** instruccion, int cantidad_ya_iniciada, int longitud, int id_patota){
	t_tripulante* tripulante = malloc(sizeof(t_tripulante));
	tripulante->TID = id_ultimo_tripulante++;
	id_ultimo_tripulante++;
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

	return tripulante;
}
