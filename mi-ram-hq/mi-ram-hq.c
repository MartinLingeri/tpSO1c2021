#include "mi-ram-hq.h"

pthread_mutex_t discordiador;

int main(void) {
	void iterator(char* value)
	{
		printf("%s\n", value);
	}

	logger = log_create("log.log", "Mi-Ram-HQ", 1, LOG_LEVEL_DEBUG);

	int server_fd = iniciar_servidor();
	log_info(logger, "Servidor listo para recibir al cliente");
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
	return EXIT_SUCCESS;
}

void enviar_mensaje(char* mensaje, int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
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

void enviar_tarea(char *tarea){
		t_buffer *buffer = serializar_enviar_tarea(tarea);
		t_paquete* paquete_enviar_tarea = crear_mensaje(buffer, PEDIR_SIGUIENTE_TAREA);
		pthread_mutex_lock(&discordiador);
		enviar_paquete(paquete_enviar_tarea, conexion_hq);
		pthread_mutex_unlock(&discordiador);
		free(buffer);
		free(paquete_enviar_tarea);
}
