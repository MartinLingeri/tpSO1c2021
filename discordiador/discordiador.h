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
int TID;
int PID;
int pos_x;
int pos_y;
estado estado;
}t_tripulante;

typedef struct {
t_tripulante* tripulante;
pthread_t hilo;
}t_tripulante_hilo;

t_log* iniciar_logger(void);
t_config* leer_config(void);
void leer_consola(t_log*);
void paquete(int);
void terminar_programa(int, t_log*, t_config*);
void iniciar_patota(char** instruccion, char* leido);
void iniciar_patota_en_hq();
t_tripulante* inicializar_tripulante(char** instruccion, int cantidad_ya_iniciada, int longitud, int id_patota);
int longitud_instr(char** instruccion);

#endif /* DISCORDIADOR_H_ */
