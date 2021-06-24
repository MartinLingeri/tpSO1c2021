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

	buffer = recibir_buffer(&size, socket_cliente);
	memcpy(&(tripulante->tid), buffer+desplazamiento, sizeof(uint32_t));
	desplazamiento+=sizeof(uint32_t);
	memcpy(&(tripulante->estado), buffer+desplazamiento, sizeof(char));
	desplazamiento+=sizeof(char);
	memcpy(&(tripulante->pos_x), buffer+desplazamiento, sizeof(uint32_t));
	desplazamiento+=sizeof(uint32_t);
	memcpy(&(tripulante->pos_y), buffer+desplazamiento, sizeof(uint32_t));
	desplazamiento+=sizeof(uint32_t);
	/*memcpy(&(tripulante->proxima_instruccion), buffer+desplazamiento, sizeof(uint32_t));//DISCORDIADOR: creemos que esto no se lo mandamos nosotros
	desplazamiento+=sizeof(uint32_t);*/
	memcpy(&(tripulante->pcb), buffer+desplazamiento, sizeof(uint32_t));
	free(buffer);
	return tripulante;
}

t_pcb* recibir_pcb(int socket_cliente){
	puts("RECIBE PCB");
	int size;
	int desplazamiento = 0;
	void* buffer;
	t_pcb* patota = malloc(sizeof(t_pcb));

	buffer = recibir_buffer(&size, socket_cliente);

	memcpy(&(patota->pid), (buffer+desplazamiento), sizeof(uint32_t));
	desplazamiento+=sizeof(uint32_t);

	void* cantidad_tripulantes = malloc(sizeof(uint32_t));

	memcpy(&cantidad_tripulantes, buffer+desplazamiento, sizeof(uint32_t));
	desplazamiento+=sizeof(uint32_t);

	void* tareas_len = malloc(sizeof(uint32_t));
	memcpy(&tareas_len, (buffer+desplazamiento), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	patota->tareas = malloc(tareas_len);
	memcpy(patota->tareas, buffer+desplazamiento, tareas_len);
	desplazamiento += tareas_len;

	free(buffer);
	return patota;
	/*CHEQUEAR QUE SE VAN A PODER GUARDAR TODOS LOS TRIPULANTES Y MANDAR SI ESTA OK O SI NO SE VAN A PODER GUARDAR EN MEMORIA*/
}

void recibir_pedir_tarea(int socket_cliente) {
	int size;
	void* buffer;

	buffer = recibir_buffer(&size, socket_cliente);

	void* cantidad_tripulantes = malloc(sizeof(uint32_t));

	memcpy(&(cantidad_tripulantes), (buffer), sizeof(uint32_t));

	free(buffer);
	/*DEVOLVER PROXIMA TAREA, SI ERA LA ULTIMA Y NO HAY MAS, CHAR* VACIO*/
}

void recibir_cambio_estado(int socket_cliente) {
	int size;
	void* buffer;
	int desplazamiento = 0;
	buffer = recibir_buffer(&size, socket_cliente);

	void* tid = malloc(sizeof(uint32_t));
	memcpy(&(tid), (buffer+desplazamiento), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	void* nuevo_estado = malloc(sizeof(char));
	memcpy(&(nuevo_estado), (buffer+desplazamiento), sizeof(char));

	free(buffer);
	/*DEVOLVER PROXIMA TAREA, SI ERA LA ULTIMA Y NO HAY MAS, CHAR* VACIO*/
}

void recibir_desplazamiento(int socket_cliente) {
	int size;
	void* buffer;
	int desplazamiento = 0;
	buffer = recibir_buffer(&size, socket_cliente);

	void* tid = malloc(sizeof(uint32_t));
	memcpy(&(tid), (buffer), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	printf("tid: %d\n", tid);

	void* nuexo_x = malloc(sizeof(uint32_t));
	memcpy(&(nuexo_x), (buffer), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	printf("nuevo x: %d\n", tid);

	void* nuexo_y = malloc(sizeof(uint32_t));
	memcpy(&(nuexo_y), (buffer), sizeof(uint32_t));
	printf("nuevo y: %d\n", tid);

	free(buffer);
}

void recibir_eliminar_tripulante(int socket_cliente) {
	int size;
	void* buffer;
	printf("antes d expulsar");
	buffer = recibir_buffer(&size, socket_cliente);

	void* id = malloc(sizeof(uint32_t));

	memcpy(&(id), (buffer), sizeof(uint32_t));
	printf("EL ID A EXP: %d\n", id);

	free(buffer);
}

t_buffer *serializar_enviar_tarea(char *tarea) {
		t_buffer* buffer=malloc(sizeof(t_buffer));
		void* stream=malloc(sizeof(uint32_t)+strlen(tarea)+1);
		int desplazamiento =0;

		void* tarea_len=malloc(sizeof(uint32_t));
		tarea_len=strlen(tarea)+1;
		memcpy(stream+desplazamiento,(void*)(&tarea_len), sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);

		memcpy(stream+desplazamiento, &tarea, tarea_len);
		desplazamiento += tarea_len;

		buffer->size=desplazamiento;
		buffer->stream = stream;
		return buffer;
}
