#include "mi-ram-hq.h"


pthread_mutex_t discordiador;

int main(void) {

	logger = iniciar_logger();
	config = leer_config();
/*
	//Carga si es PAGINACION o SEGMENTACION del archivo de config
	char* esquema_memoria = config_get_string_value(config, "ESQUEMA_MEMORIA");

	//Carga el tamaño de memoria a instanciar
	int tamanio_memoria = config_get_int_value(config, "TAMANIO_MEMORIA");

	//Hace la locacion de la memoria que se va a utilizar
	inicio_memoria = malloc(tamanio_memoria); //Puntero la primer ubicacion de memoria
													//del segmento reservado para memoria

	t_list* lista_de_segmentos = list_create();
	mostrar_lista_de_segmentos(lista_de_segmentos);

	//Dibuja el mapa inicial vacío
	NIVEL* nivel;

	int cols, rows;
	int err;

	nivel_gui_inicializar();

	nivel_gui_get_area_nivel(&cols, &rows);

	nivel = nivel_crear("A-MongOs");
*/

	//Conecta con el servidor

	void iterator(char* value)
	{
		printf("%s\n", value);
	}

	int server_fd = iniciar_servidor();
	log_info(logger, "Servidor listo para recibir al cliente");
	int cliente_fd = esperar_cliente(server_fd);
	t_tcb* tripulante = malloc(sizeof(t_tcb));
	t_pcb* patota = malloc(sizeof(t_pcb));
	int conexion = crear_conexion("127.0.0.1", "5002");
	while(1)
	{
		int cod_op = recibir_operacion(cliente_fd);
		switch(cod_op)
		{
		case TCB_MENSAJE:
			tripulante = recibir_tcb(cliente_fd);
			//mostrar_tcb(tripulante);
			break;
		case PCB_MENSAJE:
			patota = recibir_pcb(cliente_fd);
			puts("Respondiendo a DS");
			uint32_t a = 1; //1 si hay lugar 0 si no
			t_buffer* buffer = serializar_test(a);
			t_paquete* paquete = crear_mensaje(buffer, 1);
			enviar_paquete(paquete, conexion);
			break;

		case PEDIR_SIGUIENTE_TAREA:
			recibir_pedir_tarea(cliente_fd);
			break;
		case CAMBIO_ESTADO_MENSAJE:
			recibir_cambio_estado(cliente_fd);
			break;
		case DESPLAZAMIENTO:
			recibir_desplazamiento(cliente_fd);
			break;
		case ELIMINAR_TRIPULANTE:
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
	close(conexion);
	free(inicio_memoria);
	//terminar_programa(/*conexion_disc,*/ logger, config);

	return EXIT_SUCCESS;
}

int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1) {
		return -1;
	}

	freeaddrinfo(server_info);
	return socket_cliente;
}

t_buffer* serializar_test(uint32_t dato)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	void* stream = malloc(sizeof(uint32_t));
	int desplazamiento = 0;
	memcpy(stream + desplazamiento, &dato, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	buffer->size = desplazamiento;
	buffer->stream = stream;
	return buffer;
}

/*
void terminar_programa(int conexion_disc,t_log* logger, t_config* config)
{
	//liberar_conexion(conexion_disc);
	log_destroy(logger);
	config_destroy(config);

}
*/

void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void * msg = malloc(bytes);
	int desplazamiento = 0;

	memcpy(msg + desplazamiento, &(paquete->codigo_operacion), sizeof(uint8_t));
	desplazamiento+= sizeof(uint8_t);
	memcpy(msg + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(msg + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return msg;
}



