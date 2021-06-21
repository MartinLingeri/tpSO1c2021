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
#include <math.h>

#include "utils.h"

typedef struct {
	t_tripulante* tripulante;
}t_circular_args;

t_log* iniciar_logger(void);
t_config* leer_config(void);
void leer_consola(t_log* logger);
void terminar_programa(int conexion_hq, int conexion_store, t_log* logger, t_config* config);
int longitud_instr(char** instruccion);
void iniciar_patota(char** instruccion, char* leido);
void iniciar_tripulante_en_hq(t_tripulante* tripulante);
void enviar_cambio_estado_hq(t_tripulante* tripulante);
void inicializar_tripulante(char** instruccion, int cantidad_ya_iniciada, int longitud, int id_patota, pthread_t hilo);
void circular(void* args);
void cambiar_estado(int estado_anterior, int estado_nuevo, t_tripulante* tripulante);
void leer_tarea(t_tripulante* tripulante, char* tarea, int retardo_ciclo_cpu);
void planificador(void* args);
void listar_tripulantes();
void conexion_con_hq();
void conexion_con_store();
void logear_despl(int pos_x, int pos_y, char* pos_x_nuevo, char* pos_y_nuevo, int id, int conexion_hq);
void reportar_bitacora(char* log, int id, int conexion_store);
void expulsar_tripulante(char* id);

void atender_sabotaje(t_sabotaje* datos);
void mover_trips(int nuevo_estado);
void desbloquear_trips_inverso(t_list* lista);
void pasar_menor_id(t_list* lista, int estado_nuevo);
void pasar_ultimo(t_list* lista, int nuevo);
t_tripulante* tripulante_mas_cercano(int x, int y);
static void* menor_ID(t_tripulante* t1, t_tripulante* t2);
double distancia(t_tripulante* trip, int x, int y);
void resolver_sabotaje(t_tripulante* asignado, t_sabotaje* datos);

#endif /* DISCORDIADOR_H_ */
