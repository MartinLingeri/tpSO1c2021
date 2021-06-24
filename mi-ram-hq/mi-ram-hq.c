#include "mi-ram-hq.h"

pthread_mutex_t discordiador;

int main(void) {

	logger = iniciar_logger();
	config = leer_config();

	//Carga si es PAGINACION o SEGMENTACION del archivo de config
	char* esquema_memoria = config_get_string_value(config, "ESQUEMA_MEMORIA");

	//Carga el tamaño de memoria a instanciar
	int tamanio_memoria = config_get_int_value(config, "TAMANIO_MEMORIA");

	//Hace la locacion de la memoria que se va a utilizar
	void* inicio_memoria = malloc(tamanio_memoria); //Puntero la primer ubicacion de memoria
													//del segmento reservado para memoria

	//Dibuja el mapa inicial vacío
	NIVEL* nivel;

	int cols, rows;
	int err;

	nivel_gui_inicializar();

	nivel_gui_get_area_nivel(&cols, &rows);

	nivel = nivel_crear("A-MongOs");

	//Conecta con el servidor

	void iterator(char* value)
	{
		printf("%s\n", value);
	}

	int server_fd = iniciar_servidor();
	log_info(logger, "Servidor listo para recibir al client-e");
	int cliente_fd = esperar_cliente(server_fd);
	t_tcb* tripulante = malloc(sizeof(t_tcb));
	t_pcb* patota = malloc(sizeof(t_pcb));
	while(1)
	{
		printf("socket: %d\n", cliente_fd);
		int cod_op = recibir_operacion(cliente_fd);
		switch(cod_op)
		{
		case TCB_MENSAJE:
			tripulante = recibir_tcb(cliente_fd);
			//mostrar_tcb(tripulante);
			break;
		case PCB_MENSAJE:
			printf("COD OP: %d\n", cod_op);
			patota = recibir_pcb(cliente_fd);
			//mostrar_tcb(patota);
			break;
		case PEDIR_SIGUIENTE_TAREA:
			recibir_pedir_tarea(cliente_fd);
			break;
		case CAMBIO_ESTADO_MENSAJE:
			recibir_cambio_estado(cliente_fd);
			break;
		case DESPLAZAMIENTO:
			printf("COD OP: %d\n", cod_op);
			printf("entra en msj desplazamiento");
			recibir_desplazamiento(cliente_fd);
			break;
		case ELIMINAR_TRIPULANTE:
			printf("COD OP: %d\n", cod_op);
			printf("entra en msj ELIM TRIP");
			recibir_eliminar_tripulante(cliente_fd);
			break;
		case -1:
			log_error(logger, "El cliente se desconecto. Terminando servidor");
			return EXIT_FAILURE;
		default:
			break;
		}
	}
	free(tripulante);
	free(patota);

	free(inicio_memoria);
	terminar_programa(/*conexion_disc,*/ logger, config);

	return EXIT_SUCCESS;
}

void terminar_programa(/*int conexion_disc,*/t_log* logger, t_config* config)
{
	//liberar_conexion(conexion_disc);
	log_destroy(logger);
	config_destroy(config);
}

void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}
/*
void enviar_tarea(char *tarea){
		t_buffer *buffer = serializar_enviar_tarea(tarea);
		t_paquete* paquete_enviar_tarea = crear_mensaje(buffer, PEDIR_SIGUIENTE_TAREA);
		pthread_mutex_lock(&discordiador);
		enviar_paquete(paquete_enviar_tarea, conexion_hq);
		pthread_mutex_unlock(&discordiador);
		free(buffer);
		free(paquete_enviar_tarea);
		//t_paquete* paquete_enviar_tarea = crear_mensaje(buffer, PEDIR_SIGUIENTE_TAREA);
		pthread_mutex_lock(&discordiador);
		//enviar_paquete(paquete_enviar_tarea, conexion_hq);
		pthread_mutex_unlock(&discordiador);
		free(buffer);
		//free(paquete_enviar_tarea);
}*/
