#include "utils.h"


int quantum = 0;
int conexion_hq = -1;
int conexion_store = -1;

void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void * buffer_serializado = malloc(bytes);
	int desplazamiento = 0;

	memcpy(buffer_serializado + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(buffer_serializado + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(buffer_serializado + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return buffer_serializado;
}

void enviar_paquete(t_paquete* paquete, int socket_cliente)
{
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);
	//printf("CODIGO OP: %d\n", paquete->codigo_operacion);
	//printf("SIZE DEL BUFFER: %d\n", paquete->buffer->size);
	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
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

t_paquete* crear_mensaje(t_buffer* buffer, op_code codigo)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = codigo;
	paquete->buffer = buffer;
	return paquete;
}

void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

int esperar_cliente(int socket_servidor)
{
	struct sockaddr_in dir_cliente;
	uint32_t tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	log_info(logger, "Se conecto un cliente!");

	return socket_cliente;
}

int iniciar_servidor(char* ip, char* puerto)
{
	int socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(ip, puerto, &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next)
    {
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            continue;
        }
        break;
    }

	listen(socket_servidor, SOMAXCONN);
    freeaddrinfo(servinfo);

    log_trace(logger, "Listo para escuchar a mi cliente");
    return socket_servidor;
}


void mover_a(t_tripulante* tripulante, bool es_x, int valor_nuevo, int retardo_ciclo_cpu) {
      if(es_x) {
			if(tripulante->pos_x < valor_nuevo) {
				tripulante->pos_x++;
			} else {
				tripulante->pos_x--;
			}
			sleep(retardo_ciclo_cpu);

      } else {
  			if(tripulante->pos_y < valor_nuevo) {
  				tripulante->pos_y++;
  			} else {
  				tripulante->pos_y--;
  			}
  			sleep(retardo_ciclo_cpu);
      }
}

char estado_a_char(int estado){
   switch(estado){
    case e_llegada:
        return 'N';
        break;
    case e_listo:
        return 'R';
        break;
    case e_fin:
        return 'E';
        break;
    case e_trabajando:
        return 'W';
        break;
    case e_bloqueado_IO:
        return 'B';
        break;
    case e_bloqueado_emergencia:
        return 'B';
        break;
    default:
    	return '?';
    	break;
   }
}

int atoi_tarea(char* tarea){
	if(strcmp(tarea, "GENERAR_OXIGENO") == 0) {
		return GENERAR_OXIGENO;
	} else if (strcmp(tarea, "CONSUMIR_OXIGENO") == 0) {
		return CONSUMIR_OXIGENO;
	} else if (strcmp(tarea, "GENERAR_COMIDA") == 0) {
		return GENERAR_COMIDA;
	} else if (strcmp(tarea, "CONSUMIR_COMIDA") == 0) {
		return CONSUMIR_COMIDA;
	} else if (strcmp(tarea, "GENERAR_BASURA") == 0) {
		return GENERAR_BASURA;
	} else if (strcmp(tarea, "DESCARTAR_BASURA") == 0) {
		return DESCARTAR_BASURA;
	} else {
		return -1;
	}
}

int longitud_instr(char** instruccion) {
	int largo = 0;
	int i = 0;
	while(instruccion[i] != NULL){
		largo++;
		i++;
	}
	return largo;
}

double distancia(t_tripulante* trip, int x, int y){
    double valor = ((x - trip->pos_x)*(x - trip->pos_x) + (y - trip->pos_y)*(y - trip->pos_y));
    return sqrt(valor);
}


t_tripulante* buscar_tripulante(int id){
	bool es_el_tripulante(void* tripulante_en_lista) {
		return ((t_tripulante*)tripulante_en_lista)->TID == id;
	}
	if(list_any_satisfy(llegada, es_el_tripulante)){
		t_tripulante* tripulante = list_find(llegada,es_el_tripulante);
	    return tripulante;
	}else{
		t_tripulante* tripulante = list_find(listo,es_el_tripulante);
	    return tripulante;
	}
}


void terminar_programa(int conexion_hq, int conexion_store, t_log* logger, t_config* config){
	void destruir_tripulante(t_tripulante* t){
		sem_destroy(&(t->semaforo));
	}

	liberar_conexion(conexion_hq);
	liberar_conexion(conexion_store);
	config_destroy(config);
	sem_destroy(&planif);
	sem_destroy(&multiprog);
	sem_destroy(&recibido_hay_lugar);
	sem_destroy(&listo_para_trabajar);
	list_destroy_and_destroy_elements(llegada,(void*)destruir_tripulante);
	list_destroy_and_destroy_elements(listo,(void*)destruir_tripulante);
	list_destroy_and_destroy_elements(fin,(void*)destruir_tripulante);
	list_destroy_and_destroy_elements(trabajando,(void*)destruir_tripulante);
	list_destroy_and_destroy_elements(bloqueado_IO,(void*)destruir_tripulante);
	list_destroy_and_destroy_elements(bloqueado_emergencia,(void*)destruir_tripulante);
}
