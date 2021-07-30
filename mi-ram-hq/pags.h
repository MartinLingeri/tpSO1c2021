/*
 * pags.h
 *
 *  Created on: 28 jul. 2021
 *      Author: utnso
 */

#ifndef PAGS_H_
#define PAGS_H_

#include "utils.h"
#include "mlogs.h"

#include<time.h>
#include<locale.h>
#include<locale.h>
#include<commons/string.h>
#include<commons/temporal.h>


typedef struct{
	uint32_t idPatota;
	uint32_t cantPaginas;
	t_list *paginas;
}t_tabla_de_paginas;

typedef struct{
	uint32_t nroFrame;
	uint32_t espacioLibre;
	void *inicio;
	t_list *datos;
}t_frame;

typedef struct{
	uint32_t nroPagina;
	uint32_t bitPresencia;
	uint32_t bitUso;
	uint32_t bitModificado;
	uint32_t ultimoUso;
	t_frame *frame;
}t_pagina;

typedef struct{
	tipo_contenido tipoContenido;
	t_pcb *pcb;
	char *tareas;
	t_tcb *tcb;
}t_dato_en_frame;

t_list *listaDeTablasDePaginas;
t_list *listaDeFrames;
t_list *listaDeFramesSwap;

uint32_t nroPagGlobal;
uint32_t tamanioPagina;

void *inicio_memoria_virtual;

void crear_lista_de_frames(void*, uint32_t);
void crear_lista_de_frames_swap(void*, uint32_t);
void vaciar_lista_de_frames(t_list*);
void cargar_pcb_a_frame(t_frame*, t_pcb*);
void cargar_tareas_a_frame(t_frame*, char*);
void cargar_tcb_a_frame(t_frame*, t_tcb*);
int crear_tabla_de_paginas(uint32_t);
void crear_pagina(t_tabla_de_paginas*, t_frame*);
t_frame *encontrar_frame_vacio();
t_frame *encontrar_frame_disponible(t_tabla_de_paginas*, uint32_t);
void cargar_pcb_paginacion(uint32_t);
t_tabla_de_paginas *encontrar_pid_lista_tablas(uint32_t);
void cargar_tareas_paginacion(uint32_t, char*);
void cargar_tripulante_paginacion(t_tcb*);
char* tarea_indice(char*, uint32_t);
char* proxima_instruccion_tripulante_paginacion(uint32_t);
void eliminar_tripulante_paginacion(uint32_t tid);
void modificar_posicion_tripulante(uint32_t, uint32_t, uint32_t);
void modificar_estado_tripulante(uint32_t, char);
bool hay_espacio_disponible(uint32_t, uint32_t);
void dump_memoria();
t_pagina* pagina_a_remover(t_list* paginas_en_memoria);
uint32_t clock_algoritmo(t_pagina* t);
t_pagina* buscar_pagina(int numero);
t_pagina* buscar_en_swap();
t_list* remover_de_swap();
void cargar_a_swap(t_pagina*, t_frame*);
void cargar_a_memoria(t_list*, t_frame*);
void cargar_pcb_a_swap(t_frame *frame, t_pcb *pcb);
void cargar_tareas_a_swap(t_frame *frame, char *d);
void cargar_tcb_a_swap(t_frame *frame, t_tcb *d);
t_list* paginas_en_memoria();

#endif /* PAGS_H_ */
