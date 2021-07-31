#include "mi-ram-hq.h"

pthread_mutex_t discordiador;
char* esquema_memoria;
int tamanio_memoria;

int main(void) {

	logger = iniciar_logger();
	config = leer_config();
	logear(INICIO_SISTEMA,0);

	struct sigaction sig = {0};
	sig.sa_flags = SA_RESTART;
	sig.sa_handler = &dump_memoria;
	sigaction(SIGUSR1, &sig, NULL);

	esquema_memoria = config_get_string_value(config, "ESQUEMA_MEMORIA");
	tamanio_memoria = config_get_int_value(config, "TAMANIO_MEMORIA");

	inicio_memoria = malloc(tamanio_memoria); //Puntero la primer ubicacion de memoria /del segmento reservado para memoria

	//t_list* lista_de_segmentos = list_create();
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
	conexion = crear_conexion("127.0.0.1", "5002"); //IP Y PUERTO DS PONER EN CONFIG

	if(strcmp(esquema_memoria,"PAGINACION")==0){
		int tamanio_swap=config_get_int_value(config, "TAMANIO_SWAP");
		char* path_swap=config_get_string_value(config, "PATH_SWAP");
		int archivo_swap=open(path_swap,O_RDWR|O_CREAT,S_IRUSR|S_IWUSR);
		if(archivo_swap==-1){
			log_error(logger,"Error al abrir el archivo de swap");
			return EXIT_FAILURE;
		}
		ftruncate(archivo_swap,tamanio_swap);
		void* inicio_memoria_virtual=mmap(NULL,tamanio_swap,PROT_READ|PROT_WRITE,MAP_SHARED,archivo_swap,0);
		if(inicio_memoria_virtual==MAP_FAILED){
			log_error(logger,"Error al mapear en memoria el archivo de swap");
			return EXIT_FAILURE;
		}
		tamanioPagina = config_get_int_value(config,"TAMANIO_PAGINA");
		listaDeTablasDePaginas=list_create();
		crear_lista_de_frames(inicio_memoria, tamanio_memoria);
		crear_lista_de_frames_swap(inicio_memoria_virtual,tamanio_swap);
		nroPagGlobal=0;
	}else if(strcmp(esquema_memoria,"SEGMENTACION")==0){
		//PREPARACIÓN SEGMENTACION
	}else{
		log_error(logger,"Esquema de memoria no valido");
		return EXIT_FAILURE;
	}

    pthread_t hilo_discordiador;
    pthread_create(&hilo_discordiador, NULL, (void*) atender_pedidos, NULL);
    pthread_join((pthread_t) hilo_discordiador, NULL);

	terminar_programa();

	return EXIT_SUCCESS;
}

void atender_pedidos(){
	t_tcb* tripulante = malloc(sizeof(t_tcb));
	t_iniciar_patota* iniciarPatota = malloc(sizeof(t_iniciar_patota));
	uint32_t tid;
	t_paquete* paquete;
	t_buffer* buffer;

	int server_fd = iniciar_servidor();
	log_info(logger, "Servidor listo para recibir al cliente");
	int cliente_fd = esperar_cliente(server_fd);

	if(strcmp(esquema_memoria,"PAGINACION")==0){
		while(1){
			puts("Esperando discordiador");
			int cod_op = recibir_operacion(cliente_fd);
			switch(cod_op){
				case TCB_MENSAJE:
					tripulante = recibir_tcb(cliente_fd);
	                pthread_t hilo_tr;
	                pthread_create(&hilo_tr, NULL, (void*) cargar_tripulante_paginacion, tripulante);
	                pthread_detach((pthread_t) hilo_tr);
					break;

				case PCB_MENSAJE:
					iniciarPatota = recibir_pcb(cliente_fd);
					if(hay_espacio_disponible(iniciarPatota->cantTripulantes,strlen(iniciarPatota->tareas))){
                        pthread_t hilo_p;
                        pthread_create(&hilo_p, NULL, (void*) cargar_pcb_paginacion, iniciarPatota);
                        pthread_detach((pthread_t) hilo_p);
					}else{
						buffer=serializar_hay_lugar_memoria(0);
						paquete=crear_mensaje(buffer,1);
						enviar_paquete(paquete,conexion);
						logear(NO_MEMORIA,0);
					}
					break;

				case PEDIR_SIGUIENTE_TAREA:
					tid = recibir_pedir_tarea(cliente_fd);
                    pthread_t hilo_tar;
	                pthread_create(&hilo_tar, NULL, (void*) proxima_instruccion_tripulante_paginacion, tid);
	                pthread_detach((pthread_t) hilo_tar);
					break;

				case CAMBIO_ESTADO_MENSAJE:
					tripulante = recibir_cambio_estado(cliente_fd);
                    pthread_t hilo_ce;
                    pthread_create(&hilo_ce, NULL, (void*) modificar_estado_tripulante, tripulante);
                    pthread_detach((pthread_t) hilo_ce);
					break;

				case DESPLAZAMIENTO:
					tripulante=recibir_desplazamiento(cliente_fd);
                    pthread_t hilo_des;
                    pthread_create(&hilo_des, NULL, (void*) modificar_posicion_tripulante, tripulante);
                    pthread_detach((pthread_t) hilo_des);
					break;

				case ELIMINAR_TRIPULANTE:
					tid=recibir_eliminar_tripulante(cliente_fd);
                    pthread_t hilo_e;
	                pthread_create(&hilo_e, NULL, (void*) eliminar_tripulante_paginacion, tid);
	                pthread_detach((pthread_t) hilo_e);
					break;

				case -1:
					log_error(logger, "El cliente se desconecto. Terminando servidor");
					return;

				default:
					break;
				}
			}
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
					break;

				default:
					break;
				}
			}
		}else{
			log_error(logger,"Esquema de memoria no valido");
		}
	free(tripulante);
	free(iniciarPatota);
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
	if(strcmp(esquema_memoria,"PAGINACION")==0){
		vaciar_lista_de_frames(listaDeFrames);
		vaciar_lista_de_frames(listaDeFramesSwap);
	}
	free(inicio_memoria);
	close(conexion);
	logear(FIN_HQ,0);
	log_destroy(logger);
	config_destroy(config);
}

void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}
