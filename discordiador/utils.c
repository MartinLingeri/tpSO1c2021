#include "utils.h"


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

t_buffer* serilizar_patota(uint32_t id, char* tareas, uint32_t trips)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	void* stream = malloc(sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + strlen(tareas) + 1);

	int desplazamiento = 0;
	memcpy(stream + desplazamiento, &id, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &trips, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	void* tareas_len = malloc(sizeof(uint32_t));
	tareas_len = strlen(tareas) + 1;
	memcpy(stream + desplazamiento, (void*)(&tareas_len), sizeof(uint32_t));
	desplazamiento += sizeof(strlen(tareas) + 1);

	memcpy(stream + desplazamiento, tareas, strlen(tareas) + 1);
	desplazamiento += strlen(tareas) + 1;

	buffer->size = desplazamiento;
	buffer->stream = stream;
	return buffer;
}

t_buffer* serilizar_tripulante(uint32_t id, uint32_t pid, uint32_t pos_x, uint32_t pos_y, uint32_t estado)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	void* stream = malloc(sizeof(uint32_t) * 4 + sizeof(char));
	int desplazamiento = 0;

	memcpy(stream + desplazamiento, &id, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	char e = estado_a_char(estado);

	memcpy(stream + desplazamiento, &e, sizeof(char));
	desplazamiento += sizeof(char);

	memcpy(stream + desplazamiento, &pos_x, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &pos_y, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &pid, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	buffer->size = desplazamiento;
	buffer->stream = stream;
	return buffer;
}

t_buffer* serilizar_cambio_estado(uint32_t id, uint32_t estado)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	void* stream = malloc(sizeof(uint32_t) + sizeof(char));
	int desplazamiento = 0;

	memcpy(stream + desplazamiento, &id, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	char e = estado_a_char(estado);

	memcpy(stream + desplazamiento, &e, sizeof(char));
	desplazamiento += sizeof(char);

	buffer->size = desplazamiento;
	buffer->stream = stream;
	return buffer;
}

t_buffer* serilizar_pedir_tarea(uint32_t id)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	void* stream = malloc(sizeof(uint32_t));
	int desplazamiento = 0;

	memcpy(stream + desplazamiento, &id, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	buffer->size = desplazamiento;
	buffer->stream = stream;
	return buffer;
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

char* recibir_tarea(int socket_cliente) {
	int size;
	int desplazamiento = 0;
	void* buffer;

	buffer = recibir_buffer(&size, socket_cliente);

	void* tarea_len = malloc(sizeof(uint32_t));
	memcpy(&tarea_len, buffer+desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	char* tarea = malloc(tarea_len);
	memcpy(tarea, buffer+desplazamiento, tarea_len);

	return tarea;
}

void mover_a(t_tripulante* tripulante, bool es_x, int valor_nuevo, int retardo_ciclo_cpu) {
      if(es_x) {
		while(tripulante->pos_x != valor_nuevo) {
			if(tripulante->pos_x < valor_nuevo) {
				tripulante->pos_x++;
			} else {
				tripulante->pos_x--;
			}
			sleep(retardo_ciclo_cpu);
		}
		printf("x: %d\n", tripulante->pos_x);

      } else {
  		while(tripulante->pos_y != valor_nuevo) {
  			if(tripulante->pos_y < valor_nuevo) {
  				tripulante->pos_y++;
  			} else {
  				tripulante->pos_y--;
  			}
  			sleep(retardo_ciclo_cpu);
  		}
  		printf("y: %d\n", tripulante->pos_y);
      }
}

t_buffer* serializar_reporte_bitacora(uint32_t id, char* reporte)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	void* stream = malloc(sizeof(uint32_t) + sizeof(uint32_t) + strlen(reporte) + 1);
	int desplazamiento = 0;

	memcpy(stream + desplazamiento, &id, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	void* reporte_len = malloc(sizeof(uint32_t));
	reporte_len = strlen(reporte) + 1;
	memcpy(stream + desplazamiento, (void*)(&reporte_len), sizeof(uint32_t));
	desplazamiento += sizeof(strlen(reporte) + 1);
	memcpy(stream + desplazamiento, reporte, strlen(reporte) + 1);
	desplazamiento += strlen(reporte) + 1;

	buffer->size = desplazamiento;
	buffer->stream = stream;
	return buffer;
}

t_buffer* serilizar_desplazamiento(uint32_t tid, uint32_t x_nuevo, uint32_t y_nuevo)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	void* stream = malloc(sizeof(uint32_t)*3);
	int desplazamiento = 0;

	memcpy(stream + desplazamiento, &tid, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &x_nuevo, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &y_nuevo, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	buffer->size = desplazamiento;
	buffer->stream = stream;
	return buffer;
}

t_buffer* serilizar_hacer_tarea(uint32_t cantidad, char* tarea, uint32_t tid)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	void* stream = malloc(sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + strlen(tarea) + 1);
	int desplazamiento = 0;

	memcpy(stream + desplazamiento, &cantidad, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &tid, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	void* tarea_len = malloc(sizeof(uint32_t));
	tarea_len = strlen(tarea) + 1;
	memcpy(stream + desplazamiento, (void*)(&tarea_len), sizeof(uint32_t));
	desplazamiento += sizeof(strlen(tarea) + 1);
	memcpy(stream + desplazamiento, tarea, strlen(tarea) + 1);
	desplazamiento += strlen(tarea) + 1;

	buffer->size = desplazamiento;
	buffer->stream = stream;
	return buffer;
}

char* logs_bitacora(regs_bitacora asunto, char* dato1, char* dato2){
	int size;
	char* reporte;
	switch(asunto) {
		case B_DESPLAZAMIENTO:  //dato 1 posicion inicial en formato x|y, dato 2 posicion final en igual formato
			size = strlen(dato1) + strlen(dato2) + strlen("Se mueve de ") + strlen(" a ") + 1;
			reporte = malloc(size);
			strcpy (reporte, "Se mueve de ");
			strcat (reporte, dato1);
			strcat (reporte, " a ");
			strcat (reporte, dato2);
			return reporte;
			break;

		case INICIO_TAREA: //dato 1 nombre de tarea como string, dato 2 nada
			size = strlen(dato1) + strlen("Comienza ejecucion de la tarea ") + 1;
			reporte = malloc(size);
			strcpy (reporte, "Comienza ejecucion de la tarea ");
			strcat (reporte, dato1);
			return reporte;
			break;

		case FIN_TAREA: //dato 1 nombre de tarea, dato 2 nada
			size = strlen(dato1) + strlen("Se finaliza la tarea ") + 1;
			reporte = malloc(size);
			strcpy (reporte, "Se finaliza la tarea ");
			strcat (reporte, dato1);
			return reporte;
			break;

		case SABOTAJE:
			return "Se corre en pánico a la ubicación del sabotaje";
			break;

		case SABOTAJE_RESUELTO:
			return "Se resuelve el sabotaje";
			break;

	   default:
			return "Situación desconocida";
			break;
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
   }

}

void generar_oxigeno(int duracion, int id, int conexion_hq){  //ESTA BIEN IMPLEMENTADO ESTO CO N1 PAR. MAS? PAG 18 DE LA CONSIGNA
	t_buffer* buffer = serilizar_hacer_tarea(duracion, GENERAR_OXIGENO, id);
	t_paquete* paquete_hacer_tarea = crear_mensaje(buffer, HACER_TAREA);
	enviar_paquete(paquete_hacer_tarea, conexion_hq);
}

void descartar_oxigeno(int duracion, int id, int conexion_hq){
	t_buffer* buffer = serilizar_hacer_tarea(duracion, CONSUMIR_OXIGENO, id);
	t_paquete* paquete_hacer_tarea = crear_mensaje(buffer, HACER_TAREA);
	enviar_paquete(paquete_hacer_tarea, conexion_hq);
}

void generar_comida(int duracion, int id, int conexion_hq){
	t_buffer* buffer = serilizar_hacer_tarea(duracion, GENERAR_COMIDA, id);
	t_paquete* paquete_hacer_tarea = crear_mensaje(buffer, HACER_TAREA);
	enviar_paquete(paquete_hacer_tarea, conexion_hq);
}

void consumir_comida(int duracion, int id, int conexion_hq){
	t_buffer* buffer = serilizar_hacer_tarea(duracion, CONSUMIR_COMIDA, id);
	t_paquete* paquete_hacer_tarea = crear_mensaje(buffer, HACER_TAREA);
	enviar_paquete(paquete_hacer_tarea, conexion_hq);
}

void generar_basura(int duracion, int id, int conexion_hq){
	t_buffer* buffer = serilizar_hacer_tarea(duracion, GENERAR_BASURA, id);
	t_paquete* paquete_hacer_tarea = crear_mensaje(buffer, HACER_TAREA);
	enviar_paquete(paquete_hacer_tarea, conexion_hq);
}

void descartar_basura(int duracion, int id, int conexion_hq){
	t_buffer* buffer = serilizar_hacer_tarea(duracion, DESCARTAR_BASURA, id);
	t_paquete* paquete_hacer_tarea = crear_mensaje(buffer, HACER_TAREA);
	enviar_paquete(paquete_hacer_tarea, conexion_hq);
}

void reportar_bitacora(char* log, int id, int conexion_hq){
    t_buffer* buffer = serializar_reporte_bitacora(id, log);
	t_paquete* paquete_bitacora = crear_mensaje(buffer, REPORTE_BITACORA);
	enviar_paquete(paquete_bitacora, conexion_hq);
}

void logear_despl(int pos_x, int pos_y, char* pos_x_nuevo, char* pos_y_nuevo, int id, int conexion_hq){
	int size = sizeof(int)*2 + sizeof('|');
	char *str_start = malloc(size);
	char *str_end = malloc(size);

	char *x = malloc(sizeof(pos_x));
	char *y = malloc(sizeof(pos_y));

	string_itoa(pos_x, x, 10);
	string_itoa(pos_y, y, 10);

	strcpy (str_start, x);
	strcat (str_start, "|");
	strcat (str_start, y);

	strcpy (str_end, pos_x_nuevo);
	strcat (str_end, "|");
	strcat (str_end, pos_x_nuevo);

	reportar_bitacora(logs_bitacora(B_DESPLAZAMIENTO, str_start, str_end), id, conexion_hq);
}
