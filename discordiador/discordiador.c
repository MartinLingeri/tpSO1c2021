#include "discordiador.h"

int main(void)
{
	int conexion;
	char* ip;
	char* puerto;
	char* valor;

	t_log* logger;
	t_config* config;

	logger = iniciar_logger();

	log_info(logger, "soy un log");

	config = leer_config();

	valor = config_get_string_value(config, "CLAVE");

	log_info(logger, valor);
	puts("desp del log del valor");
	ip = config_get_string_value(config, "IP");
	puts(ip);

	puerto = config_get_string_value(config, "PUERTO");
	puts(puerto);
	conexion = crear_conexion(ip, puerto);

	if(conexion == -1) {
		log_error(logger, "error en la conexion");
		return EXIT_FAILURE;
	}

	enviar_mensaje(valor, conexion);

	paquete(conexion);

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
	puts("strings a loguear");
	//El primero te lo dejo de yapa
	leido = readline(">");
	while (strcmp(leido, "") != 0) {
		log_info(logger, leido);
		leido = readline(">");
	}
	free(leido);
}

void paquete(int conexion)
{
	//Ahora toca lo divertido!

	char* leido;
	t_paquete* paquete = crear_paquete();

	puts("strings a mandar");
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
