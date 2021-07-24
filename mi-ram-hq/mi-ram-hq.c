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

	char* bitacora_de_prueba =
			"Inicia tripulante\n Genera oxigeno \n Resuelve sabotaje\n Fin de Quantum\n Consume comida\n Fin de lista de tareas\n Fin de tripulante";


	int server_fd = iniciar_servidor();
	log_info(logger, "Servidor listo para recibir al cliente");
	int cliente_fd = esperar_cliente(server_fd);
	t_tcb* tripulante = malloc(sizeof(t_tcb));
	t_pcb* patota = malloc(sizeof(t_pcb));
	t_paquete* paquete;
	t_buffer* buffer;
	uint32_t id;
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
			buffer = serializar_hay_lugar_memoria(a);
			paquete = crear_mensaje(buffer, 1);
			enviar_paquete(paquete, conexion);
			break;

		case PEDIR_SIGUIENTE_TAREA:
			id = recibir_pedir_tarea(cliente_fd);
			puts("Respondiendo a pedir tarea");
			printf("ID A CONTESTAR: %d\n", id);
			/*buffer = serializar_tarea(id,"GENERAR_OXIGENO 12;2;3;5");
			paquete = crear_mensaje(buffer, 3);
			enviar_paquete(paquete, conexion);*/
			free(buffer);
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

		case REPORTE_BITACORA:
			recibir_rbitacora(cliente_fd);

			buffer = serializar_bitacora(bitacora_de_prueba);
			puts("a");
			paquete = crear_mensaje(buffer, 0);
			puts("b");
			enviar_paquete(paquete, conexion);
			puts("c");
			free(buffer);
			break;

		case HACER_TAREA:
			recibir_hacer_tarea(cliente_fd);
			break;

		case PEDIR_BITACORA:
			recibir_pedir_bitacora(cliente_fd);
			break;

		case INVOCAR_FSCK:
			recibir_invocar_fsck(cliente_fd);
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
	terminar_programa();

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

t_buffer* serializar_hay_lugar_memoria(uint32_t dato)
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

void terminar_programa()
{
	log_destroy(logger);
	config_destroy(config);
}

void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

t_buffer* serializar_tarea(uint32_t id, char* tarea)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	void* stream = malloc(2*sizeof(uint32_t) + sizeof(tarea) + 1);

	int desplazamiento = 0;
	uint32_t tareas_len;
	tareas_len = strlen(tarea) + 1;

	memcpy(stream + desplazamiento, (void*)(&id), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, (void*)(&tareas_len), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, tarea, strlen(tarea) + 1);
	desplazamiento += strlen(tarea) + 1;

	buffer->size = desplazamiento;
	buffer->stream = stream;
	return buffer;
}

t_buffer* serializar_sabotaje(uint32_t x, uint32_t y)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	void* stream = malloc(2*sizeof(uint32_t));

	int desplazamiento = 0;

	memcpy(stream + desplazamiento, (void*)(&x), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, (void*)(&y), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	buffer->size = desplazamiento;
	buffer->stream = stream;
	return buffer;
}

t_buffer* serializar_bitacora(char* reporte)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	void* stream = malloc(sizeof(uint32_t) + sizeof(uint32_t) + strlen(reporte) + 1);
	int desplazamiento = 0;

	uint32_t  reporte_len;
	reporte_len = strlen(reporte) + 1;
	memcpy(stream + desplazamiento, (void*)(&reporte_len), sizeof(uint32_t));
	desplazamiento += sizeof(strlen(reporte) + 1);
	memcpy(stream + desplazamiento, reporte, strlen(reporte) + 1);
	desplazamiento += strlen(reporte) + 1;

	buffer->size = desplazamiento;
	buffer->stream = stream;
	return buffer;
}
