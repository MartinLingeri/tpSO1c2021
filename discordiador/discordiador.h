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
#include "cambio_estado.h"

typedef struct {
	t_tripulante* tripulante;
}t_circular_args;

void conexion_con_hq();
void conexion_con_store();
int recibir_operacion(int socket_cliente);
void* esperar_conexion();
void leer_consola(t_log* logger);
void logear_error();
void planificador(void* args);
void iniciar_patota(char** instruccion, char* leido);
void inicializar_tripulante(char** instruccion, int cantidad_ya_iniciada, int longitud, int id_patota, pthread_t hilo);
void circular(void* args);
int leer_tarea(t_tripulante* tripulante, char* tarea, int retardo_ciclo_cpu);
void realizar_tarea(char* tarea, int duracion, t_tripulante* tripulante);
void listar_tripulantes();
void expulsar_tripulante(char* id);
void pedir_tarea(t_tripulante* tripulante);
void atender_sabotaje(t_sabotaje* datos);
void obtener_bitacora (char* i);

#endif /* DISCORDIADOR_H_ */
