#include"utils.h"

t_config* leer_config(void)
{
	return config_create("mi-ram-hq.config");
}

t_log* iniciar_logger(void)
{
	return log_create("mi-ram-hq.log", "mi-ram-hq", true, LOG_LEVEL_INFO);
}

//----------------------------

void enviar_paquete(t_paquete* paquete, int socket_cliente)
{
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);
	printf("CODIGO OP: %d\n", paquete->codigo_operacion);
	printf("SIZE DEL BUFFER: %d\n", paquete->buffer->size);
	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

t_paquete* crear_mensaje(t_buffer* buffer, op_code codigo)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = codigo;
	paquete->buffer = buffer;
	return paquete;
}

int iniciar_servidor(void)
{
	int socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    t_config* config = leer_config();
	char* ip = config_get_string_value(config, "IP");
	char* puerto = config_get_string_value(config, "PUERTO");
	printf("ip: %s", ip);
	printf("puerto: %s", puerto);
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

int esperar_cliente(int socket_servidor)
{
	struct sockaddr_in dir_cliente;
	int tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	log_info(logger, "Se conecto un cliente!");

	return socket_cliente;
}

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) != 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void* buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
	return NULL;
}

t_tcb* recibir_tcb(int socket_cliente){
	int size;
	int desplazamiento = 0;
	void* buffer;
	t_tcb* tripulante = malloc(sizeof(t_tcb));
	puts("Llega TCB tripulante");

	buffer = recibir_buffer(&size, socket_cliente);
	memcpy(&(tripulante->tid), buffer+desplazamiento, sizeof(uint32_t));
	desplazamiento+=sizeof(uint32_t);

	memcpy(&(tripulante->estado), buffer+desplazamiento, sizeof(char));
	desplazamiento+=sizeof(char);

	memcpy(&(tripulante->pos_x), buffer+desplazamiento, sizeof(uint32_t));
	desplazamiento+=sizeof(uint32_t);

	memcpy(&(tripulante->pos_y), buffer+desplazamiento, sizeof(uint32_t));
	desplazamiento+=sizeof(uint32_t);

	memcpy(&(tripulante->pcb), buffer+desplazamiento, sizeof(uint32_t));
	desplazamiento+=sizeof(uint32_t);

	printf("TID: %d\n", tripulante->tid);
	printf("ESTADO: %c\n", tripulante->estado);
	printf("POS X: %d\n", tripulante->pos_x);
	printf("ṔOS Y: %d\n", tripulante->pos_y);
	printf("PID: %d\n", tripulante->pcb);

	free(buffer);
	return tripulante;
}

t_pcb* recibir_pcb(int socket_cliente){
	int size;
	int desplazamiento = 0;
	void* buffer;
	t_iniciar_patota* patota = malloc(sizeof(t_iniciar_patota));

	buffer = recibir_buffer(&size, socket_cliente);

	memcpy(&(patota->pid), (buffer+desplazamiento), sizeof(uint32_t));
	desplazamiento+=sizeof(uint32_t);

	memcpy(&(patota->cantTripulantes), buffer+desplazamiento, sizeof(uint32_t));
	desplazamiento+=sizeof(uint32_t);

	void* tareas_len = malloc(sizeof(uint32_t));
	memcpy(&tareas_len, (buffer+desplazamiento), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(patota->tareas, buffer+desplazamiento, tareas_len);

	free(buffer);
	return patota;
}

uint32_t recibir_pedir_tarea(int socket_cliente) {
	uint32_t size;
	void* buffer;
	int tid;
	int desplazamiento = 0;
	buffer = recibir_buffer(&size, socket_cliente);

	memcpy(&(tid), (buffer+desplazamiento), sizeof(uint32_t));

	printf("tip id: %d\n", (tid));
	desplazamiento += sizeof(uint32_t);

	free(buffer);
	return tid;
}

t_tcb* recibir_cambio_estado(int socket_cliente) {
	int size;
	void* buffer;
	int desplazamiento = 0;
	buffer = recibir_buffer(&size, socket_cliente);
	t_tcb *est=malloc(sizeof(t_tcb));

	memcpy(&(est->tid), (buffer+desplazamiento), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(&(est->estado), (buffer+desplazamiento), sizeof(char));

	free(buffer);
	return est;
}

t_tcb* recibir_desplazamiento(int socket_cliente) {
	int size;
	void* buffer;
	int desplazamiento = 0;
	buffer = recibir_buffer(&size, socket_cliente);
	t_tcb *des=malloc(sizeof(t_tcb));

	memcpy(&(des->tid), (buffer), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(&(des->pos_x), (buffer), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(&(des->pos_y), (buffer), sizeof(uint32_t));

	free(buffer);
	return des;
}

uint32_t recibir_eliminar_tripulante(int socket_cliente) {
	int size;
	void* buffer;
	buffer = recibir_buffer(&size, socket_cliente);

	uint32_t *tid = malloc(sizeof(uint32_t));

	memcpy(&(tid), (buffer), sizeof(uint32_t));

	free(buffer);
	return (*tid);
}

//DE ACA PARA ABAJO SON DESERIALIZACIONES DE I-MONGO-STORE

void recibir_pedir_bitacora(int socket_cliente) {
	int size;
	void* buffer;
	uint32_t id;
	int desplazamiento = 0;

	puts("BITACORA SOLICITADA");

	buffer = recibir_buffer(&size, socket_cliente);

	memcpy(&id, buffer+desplazamiento, sizeof(uint32_t));
	desplazamiento+=sizeof(uint32_t);

	printf("TRIP A BUSCAR BITACORA: %d\n", id);

	free(buffer);
}

void recibir_invocar_fsck(int socket_cliente) {
	int size;
	void* buffer;
	uint32_t id;
	int desplazamiento = 0;

	puts("Invoca FSCK");

	buffer = recibir_buffer(&size, socket_cliente);

	memcpy(&id, buffer+desplazamiento, sizeof(uint32_t));
	desplazamiento+=sizeof(uint32_t);

	printf("TRIP QUE INVOCÓ: %d\n", id);

	free(buffer);
}


void recibir_hacer_tarea(int socket_cliente) {
	int size;
	void* buffer;
	int desplazamiento = 0;
	buffer = recibir_buffer(&size, socket_cliente);
	uint32_t cantidad;
	int tarea;

	puts("HACER TAREA");

	memcpy(&(cantidad), buffer+desplazamiento, sizeof(uint32_t));
	desplazamiento+=sizeof(uint32_t);

	memcpy(&(tarea), buffer+desplazamiento, sizeof(int));
	desplazamiento+=sizeof(int);

	printf("CANTIDAD: %d\n", cantidad);
	printf("NUMERO TAREA: %d\n", tarea);

	free(buffer);
}


void recibir_rbitacora(int socket_cliente) {
	int size;
	int desplazamiento = 0;
	void* buffer;
	uint32_t len;
	uint32_t id;
	char* reporte;
	printf("REPORTE DE BITACORA RECIBIDO\n");
	buffer = recibir_buffer(&size, socket_cliente);

	memcpy(&id, buffer+desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(&(len), (buffer+desplazamiento), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	reporte = malloc(len);
	memcpy((reporte), buffer+desplazamiento, len);
	desplazamiento += len;

	printf("TRIPULANTE: %d\n", id);
	printf("REPORTE: %s\n", reporte);

	free(buffer);
}

