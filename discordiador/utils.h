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

t_sabotaje* recibir_datos_sabotaje(int socket_cliente);
int recibir_hay_lugar(int socket_cliente);
char* recibir_bitacora(int socket_cliente);
char* recibir_tarea(int socket_cliente);

void mover_a(t_tripulante* tripulante, bool xOy, int valor_nuevo, int retardo_ciclo_cpu);
char* logs_bitacora(regs_bitacora asunto, char* dato1, char* dato2);
char estado_a_char(int estado);
int atoi_tarea(char* tarea);

#endif /* UTILS_H_ */
