#ifndef UTILS_H_
#define UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>
#include<semaphore.h>
#include<commons/collections/list.h>
#include<math.h>

t_log* logger;

pthread_mutex_t estados[6];
pthread_mutex_t store;
pthread_mutex_t hq;
pthread_mutex_t sabotaje;
pthread_mutex_t logs;
pthread_mutex_t io;

t_list* llegada;
t_list* listo;
t_list* fin;
t_list* trabajando;
t_list* bloqueado_IO;
t_list* bloqueado_emergencia;

typedef enum
{
	PCB_MENSAJE,
	TCB_MENSAJE,
	CAMBIO_ESTADO_MENSAJE,
	PEDIR_SIGUIENTE_TAREA,
	MENSAJE,
    REPORTE_BITACORA,
    DESPLAZAMIENTO,
    HACER_TAREA,
	ELIMINAR_TRIPULANTE,
	PEDIR_BITACORA,
	ELIMINAR_TRIP,
	INVOCAR_FSCK,
}op_code;

typedef enum
{
	BITACORA,
	LUGAR_MEMORIA,
	ALERTA_SABOTAJE,
	TAREA,
}recv_code;

typedef enum
{
	GENERAR_OXIGENO,
	CONSUMIR_OXIGENO,
	GENERAR_COMIDA,
	CONSUMIR_COMIDA,
	GENERAR_BASURA,
	DESCARTAR_BASURA,
}tareas;

typedef enum
{
	e_llegada,
	e_listo,
	e_fin,
	e_trabajando,
	e_bloqueado_IO,
	e_bloqueado_emergencia,
}estado;

typedef struct {
	int TID;
	int PID;
	int pos_x;
	int pos_y;
	estado estado;
	sem_t semaforo;
}t_tripulante;

typedef enum
{
    B_DESPLAZAMIENTO,
    INICIO_TAREA,
    FIN_TAREA,
    SABOTAJE,
    SABOTAJE_RESUELTO
}regs_bitacora;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

typedef struct{
    uint32_t x;
    uint32_t y;
}t_sabotaje;

void* serializar_paquete(t_paquete* paquete, int bytes);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
int crear_conexion(char* ip, char* puerto);
t_paquete* crear_mensaje(t_buffer* buffer, op_code codigo);
void eliminar_paquete(t_paquete* paquete);
void liberar_conexion(int socket_cliente);
void* recibir_buffer(int* size, int socket_cliente);

void mover_a(t_tripulante* tripulante, bool xOy, int valor_nuevo, int retardo_ciclo_cpu);
char estado_a_char(int estado);
int atoi_tarea(char* tarea);
int longitud_instr(char** instruccion);
double distancia(t_tripulante* trip, int x, int y);

#endif /* UTILS_H_ */
