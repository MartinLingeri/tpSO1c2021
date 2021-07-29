/*
 * pags.h
 *
 *  Created on: 28 jul. 2021
 *      Author: utnso
 */

#ifndef PAGS_H_
#define PAGS_H_

#include "utils.h"

#include<time.h>
#include<locale.h>

void crear_lista_de_frames(void*, t_list*, uint32_t, uint32_t);
void vaciar_lista_de_frames(t_list*);
void cargar_pcb_a_frame(t_frame*, t_pcb*);
void cargar_tareas_a_frame(t_frame*, char*);
void cargar_tcb_a_frame(t_frame*, t_tcb*);
int crear_tabla_de_paginas(t_list*, uint32_t);
void crear_pagina(t_tabla_de_paginas*, t_frame*);
t_frame *encontrar_frame_vacio(t_list*, uint32_t);
t_frame *encontrar_frame_disponible(t_tabla_de_paginas*, t_list*, uint32_t, uint32_t);
void cargar_pcb_paginacion(t_list*, t_list*, uint32_t, uint32_t);
t_tabla_de_paginas *encontrar_pid_lista_tablas(t_list*, uint32_t);
void cargar_tareas_paginacion(t_list*, t_list*, uint32_t, uint32_t, char*);
void cargar_tripulante_paginacion(t_list*, t_list*, uint32_t, t_tcb*);
char* tarea_indice(char*, uint32_t);
char* proxima_instruccion_tripulante_paginacion(t_list*, uint32_t);
void eliminar_tripulante_paginacion(t_list *listaDeTablasDePaginas, uint32_t tamanioPagina, uint32_t tid);
void modificar_posicion_tripulante(t_list*, uint32_t, uint32_t, uint32_t);
void modificar_estado_tripulante(t_list*, uint32_t, char);
void listar_tripulantes(t_list*, uint32_t);
bool hay_espacio_disponible(t_list*, uint32_t, uint32_t, uint32_t);
t_pagina* pagina_a_remover(t_list* paginas_en_memoria);
uint32_t clock_algoritmo(t_pagina* t);
t_pagina* buscar_swap(int numero);
t_pagina* buscar_en_swap();
void remover_de_swap();
void cargar_a_memoria();
void cargar_a_swap();

#endif /* PAGS_H_ */
