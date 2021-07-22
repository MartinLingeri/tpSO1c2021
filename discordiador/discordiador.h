#ifndef DISCORDIADOR_H_
#define DISCORDIADOR_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/string.h>
#include<commons/config.h>
#include<commons/collections/list.h>
#include<readline/readline.h>
#include<pthread.h>
#include<time.h>
#include<locale.h>

#include "utils.h"
#include "serializaciones.h"
#include "logs.h"
#include "sabotajes.h"

typedef struct {
	t_tripulante* tripulante;
}t_circular_args;

void conexion_con_hq();
void conexion_con_store();
t_log* iniciar_logger(void);
t_config* leer_config(void);
void terminar_programa(int conexion_hq, int conexion_store, t_log* logger, t_config* config);
int iniciar_servidor(char* ip, char* puerto);
int recibir_operacion(int socket_cliente);
int esperar_cliente(int socket_servidor);
void* esperar_conexion();
void leer_consola(t_log* logger);
void logear_error();
void planificador(void* args);
void iniciar_patota(char** instruccion, char* leido);
void iniciar_tripulante_en_hq(t_tripulante* tripulante);
void enviar_cambio_estado_hq(t_tripulante* tripulante);
void enviar_desplazamiento_hq(t_tripulante* tripulante);
void inicializar_tripulante(char** instruccion, int cantidad_ya_iniciada, int longitud, int id_patota, pthread_t hilo);
void circular(void* args);
void cambiar_estado(int estado_anterior, int estado_nuevo, t_tripulante* tripulante);
int leer_tarea(t_tripulante* tripulante, char* tarea, int retardo_ciclo_cpu);
void realizar_tarea(char* tarea, int duracion, t_tripulante* tripulante);
void listar_tripulantes();
void expulsar_tripulante(char* id);
t_tripulante* buscar_tripulante(int id);
void pedir_tarea(t_tripulante* tripulante);
void atender_sabotaje(t_sabotaje* datos);
void mover_trips(int nuevo_estado);
void desbloquear_trips_inverso(t_list* lista);
void pasar_menor_id(t_list* lista, int estado_nuevo);
void pasar_ultimo(t_list* lista, int nuevo);
t_tripulante* tripulante_mas_cercano(int x, int y);
void resolver_sabotaje(t_tripulante* asignado, t_sabotaje* datos);

void obtener_bitacora (char* i);

#endif /* DISCORDIADOR_H_ */
