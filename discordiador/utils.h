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
#include <semaphore.h>

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
}op_code;

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

void* serializar_paquete(t_paquete* paquete, int bytes);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
int crear_conexion(char* ip, char* puerto);
t_paquete* crear_mensaje(t_buffer* buffer, op_code codigo);
t_buffer* serilizar_patota(uint32_t id, char* tareas); //DEVOLVER SI HAY LUGAR, AGREGAR CANT TRIPS
t_buffer* serilizar_tripulante(uint32_t id, uint32_t pid, uint32_t pos_x, uint32_t pos_y, uint32_t estado);
t_buffer* serilizar_cambio_estado(uint32_t id, uint32_t estado);
t_buffer* serilizar_pedir_tarea(uint32_t id); //DEVOLVER TAREA O NADA
t_buffer* serilizar_reporte_bitacora(uint32_t id, char* reporte);
t_buffer* serilizar_desplazamiento(uint32_t tid, uint32_t x_nuevo, uint32_t y_nuevo);
t_buffer* serilizar_hacer_tarea(uint32_t cantidad, char* tarea); //AGREGAR TID
void eliminar_paquete(t_paquete* paquete);
void liberar_conexion(int socket_cliente);
void* recibir_buffer(int* size, int socket_cliente);
int recibir_operacion(int socket_cliente);
char* recibir_tarea(int socket_cliente);
void mover_a(t_tripulante* tripulante, bool xOy, int valor_nuevo, int retardo_ciclo_cpu);

char* logs_bitacora(regs_bitacora asunto, t_tripulante tripulante, char* dato1, char* dato2);
void logear_despl(int pos_x, int pos_y, char* pos_x_nuevo, char* pos_x_nuevo, int id);
void reportar_bitacora(char* log, int id);
void generar_oxigeno(int duracion);
void consumir_oxigeno(int duracion);
void generar_comida(int duracion);
void consumir_comida(int duracion);
void generar_basura(int duracion);
void descartar_basura(int duracion);

#endif /* UTILS_H_ */
