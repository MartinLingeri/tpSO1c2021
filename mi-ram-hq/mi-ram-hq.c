#include "mi-ram-hq.h"

pthread_mutex_t discordiador;

int main(void) {

	logger = iniciar_logger();
	config = leer_config();
	logear(INICIO_SISTEMA,0);

	struct sigaction sig = {0};
	sig.sa_flags = SA_RESTART;
	sig.sa_handler = &dump_memoria;
	sigaction(SIGUSR1, &sig, NULL);

	char* esquema_memoria = config_get_string_value(config, "ESQUEMA_MEMORIA");
	int tamanio_memoria = config_get_int_value(config, "TAMANIO_MEMORIA");

	inicio_memoria = malloc(tamanio_memoria); //Puntero la primer ubicacion de memoria
													//del segmento reservado para memoria

	t_list* lista_de_segmentos = list_create();
	//mostrar_lista_de_segmentos(lista_de_segmentos);
/*
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
	t_iniciar_patota* iniciarPatota = malloc(sizeof(t_iniciar_patota));
	uint32_t tid;
	t_paquete* paquete;
	t_buffer* buffer;

	int conexion = crear_conexion("127.0.0.1", "5002");
	if(strcmp(esquema_memoria,"PAGINACION")==0){
		uint32_t tamanio_pagina = config_get_int_value(config,"TAMANIO_PAGINA");
		listaDeTablasDePaginas=list_create();
		crear_lista_de_frames(inicio_memoria, listaDeFrames, tamanio_memoria, tamanio_pagina);
		while(1){
			int cod_op = recibir_operacion(cliente_fd);
			switch(cod_op){
				case TCB_MENSAJE:
					tripulante = recibir_tcb(cliente_fd);
					pthread_mutex_lock(&cargar);
					cargar_tripulante_paginacion(listaDeFrames, listaDeTablasDePaginas, tamanio_pagina, tripulante);
					pthread_mutex_unlock(&cargar);
					break;

				case PCB_MENSAJE:
					iniciarPatota = recibir_pcb(cliente_fd);
					if(hay_espacio_disponible(listaDeFrames, tamanio_pagina, iniciarPatota->cantTripulantes,strlen(iniciarPatota->tareas))){
						buffer=serializar_hay_lugar_memoria(1);
						paquete=crear_mensaje(buffer,1);
						enviar_paquete(paquete,conexion);
						pthread_mutex_lock(&cargar);
						cargar_pcb_paginacion(listaDeFrames, listaDeTablasDePaginas, tamanio_pagina, iniciarPatota->pid);
						cargar_tareas_paginacion(listaDeFrames, listaDeTablasDePaginas, tamanio_pagina, iniciarPatota->pid, iniciarPatota->tareas);
						pthread_mutex_unlock(&cargar);
					}else{
						buffer=serializar_hay_lugar_memoria(0);
						paquete=crear_mensaje(buffer,1);
						enviar_paquete(paquete,conexion);
						logear(NO_MEMORIA,0);
					}
					break;

				case PEDIR_SIGUIENTE_TAREA:
					tid = recibir_pedir_tarea(cliente_fd);
					char *proximaTarea=proxima_instruccion_tripulante_paginacion(listaDeTablasDePaginas, tid);
					buffer=serializar_tarea(tid,proximaTarea);
					paquete=crear_mensaje(buffer,3);
					enviar_paquete(paquete,conexion);
					logear(SOLICITA_TAREA,tid);
					break;

				case CAMBIO_ESTADO_MENSAJE:
					tripulante = recibir_cambio_estado(cliente_fd);
					modificar_estado_tripulante(listaDeTablasDePaginas,tripulante->tid, tripulante->estado);
					break;

				case DESPLAZAMIENTO:
					tripulante=recibir_desplazamiento(cliente_fd);
					modificar_posicion_tripulante(listaDeTablasDePaginas, tripulante->tid, tripulante->pos_x, tripulante->pos_y);
					break;

				case ELIMINAR_TRIPULANTE:
					tid=recibir_eliminar_tripulante(cliente_fd);
					pthread_mutex_lock(&cargar);
					eliminar_tripulante_paginacion(listaDeTablasDePaginas, tamanio_pagina, tid);
					pthread_mutex_unlock(&cargar);
					logear(ELIMINAR_TRIP,tid);
					break;

				case -1:
					log_error(logger, "El cliente se desconecto. Terminando servidor");
					return EXIT_FAILURE;

				default:
					break;
				}
			}
		vaciar_lista_de_frames(listaDeFrames);
	}else if(strcmp(esquema_memoria,"SEGMENTACION")==0){
		while(1){
			int cod_op = recibir_operacion(cliente_fd);
			switch(cod_op){
				case TCB_MENSAJE:
					tripulante = recibir_tcb(cliente_fd);
					//mostrar_tcb(tripulante);

					break;

				case PCB_MENSAJE:
					iniciarPatota = recibir_pcb(cliente_fd);
					puts("Respondiendo a DS");
					uint32_t a = 1; //1 si hay lugar 0 si no
					buffer = serializar_hay_lugar_memoria(a);
					paquete = crear_mensaje(buffer, 1);
					enviar_paquete(paquete, conexion);
					break;

				case PEDIR_SIGUIENTE_TAREA:
					tid = recibir_pedir_tarea(cliente_fd);
					puts("Respondiendo a pedir tarea");
					printf("ID A CONTESTAR: %d\n", tid);
					/*buffer = serializar_tarea(id,"GENERAR_OXIGENO 12;2;3;5");
					paquete = crear_mensaje(buffer, 3);
					enviar_paquete(paquete, conexion);*/
					free(buffer);
					logear(SOLICITA_TAREA,tid);
					break;

				case CAMBIO_ESTADO_MENSAJE:
					tripulante=recibir_cambio_estado(cliente_fd);
					break;

				case DESPLAZAMIENTO:
					tripulante=recibir_desplazamiento(cliente_fd);

					break;

				case ELIMINAR_TRIPULANTE:
					tid = recibir_eliminar_tripulante(cliente_fd);
					logear(ELIMINAR_TRIP,tid);
					break;

				case -1:
					log_error(logger, "El cliente se desconecto. Terminando servidor");
					return EXIT_FAILURE;

				default:
					break;
				}
			}
		}else{
			log_error(logger,"Esquema de memoria no valido");
		}
	free(tripulante);
	free(iniciarPatota);
	free(inicio_memoria);
	logear(FIN_HQ,0);
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
