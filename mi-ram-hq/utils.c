#include"utils.h"

t_config* leer_config(void)
{
	config_create("mi-ram-hq.config");
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
	puts("a");
	t_tcb* tripulante = malloc(sizeof(t_tcb));

	buffer = recibir_buffer(&size, socket_cliente);

	memcpy(&(tripulante->tid), buffer+desplazamiento, sizeof(uint32_t));
	desplazamiento+=sizeof(uint32_t);
	puts("b");
	memcpy(&(tripulante->estado), buffer+desplazamiento, sizeof(char));
	desplazamiento+=sizeof(char);

	memcpy(&(tripulante->pos_x), buffer+desplazamiento, sizeof(uint32_t));
	desplazamiento+=sizeof(uint32_t);

	memcpy(&(tripulante->pos_y), buffer+desplazamiento, sizeof(uint32_t));
	desplazamiento+=sizeof(uint32_t);

	memcpy(&(tripulante->proxima_instruccion), buffer+desplazamiento, sizeof(uint32_t));//DISCORDIADOR: creemos que esto no se lo mandamos nosotros
	desplazamiento+=sizeof(uint32_t);

	memcpy(&(tripulante->pcb), buffer+desplazamiento, sizeof(uint32_t));
	puts("c");
	free(buffer);
	return tripulante;
}

t_pcb* recibir_pcb(int socket_cliente){
	int size;
	int desplazamiento = 0;
	void* buffer;
	t_pcb* patota = malloc(sizeof(t_pcb));

	buffer = recibir_buffer(&size, socket_cliente);
	printf("el size del buffer: %d\n", size);

	memcpy(&(patota->pid), (buffer+desplazamiento), sizeof(uint32_t));
	desplazamiento+=sizeof(uint32_t);
	printf("patota id: %d\n", (patota->pid));

	uint32_t* x = malloc(sizeof(uint32_t));

	memcpy(&x, buffer+desplazamiento, sizeof(uint32_t));
	desplazamiento+=sizeof(uint32_t);

	printf("cant trips: %d\n", &x);

	puts("1");
	void* tareas_len = malloc(sizeof(uint32_t));
	puts("2");
	memcpy(&tareas_len, (buffer+desplazamiento), sizeof(uint32_t));
	puts("3");
	desplazamiento += sizeof(uint32_t);
	printf("el tareas len: %d\n", (size_t)tareas_len);

	patota->tareas = malloc(tareas_len);
	memcpy(patota->tareas, buffer+desplazamiento, tareas_len);
	desplazamiento += tareas_len;

	printf("tareas: %s\n", patota->tareas);

	free(buffer);
	return patota;
}
