#ifndef DISCORDIADOR_H_
#define DISCORDIADOR_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/string.h>
#include<commons/config.h>
#include<commons/collections/list.h>
#include<readline/readline.h>
#include<pthread.h>

#include "utils.h"

typedef struct {
t_tripulante* tripulante;
}t_circular_args;

t_log* iniciar_logger(void);
t_config* leer_config(void);
void leer_consola(t_log* logger);
void terminar_programa(int conexion, t_log* logger, t_config* config);
int longitud_instr(char** instruccion);
void iniciar_patota(char** instruccion, char* leido);
void iniciar_tripulante_en_hq(t_tripulante* tripulante);
void enviar_cambio_estado_hq(t_tripulante* tripulante);
void inicializar_tripulante(char** instruccion, int cantidad_ya_iniciada, int longitud, int id_patota, pthread_t hilo);
void circular(void* args);
void cambiar_estado(int estado_anterior, int estado_nuevo, t_tripulante* tripulante);

#endif /* DISCORDIADOR_H_ */
