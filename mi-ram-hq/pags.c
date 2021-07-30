/*
 * pags.c
 *
 *  Created on: 28 jul. 2021
 *      Author: utnso
 */

#include "pags.h"

void crear_lista_de_frames(void *memoria, t_list *listaDeFrames, uint32_t tamanioMemoria, uint32_t tamanioPagina){
	listaDeFrames=list_create();
	void *p=memoria;
	t_frame *frameVacio = malloc(sizeof(t_frame));
	for(uint32_t i=0; i<(tamanioMemoria/tamanioPagina);i++){
		frameVacio->nroFrame=i;
		frameVacio->espacioLibre=tamanioPagina;
		frameVacio->inicio=p;
		frameVacio->datos=list_create();
		list_add(listaDeFrames,frameVacio);
		p+=tamanioPagina;
	}
}

void crear_lista_de_frames_swap(void *memoriaSwap, t_list *listaDeFramesSwap, uint32_t tamanioSwap, uint32_t tamanioPagina){
	listaDeFramesSwap=list_create();
		void *p=memoriaSwap;
		t_frame *frameVacio = malloc(sizeof(t_frame));
		for(uint32_t i=0; i<(tamanioSwap/tamanioPagina);i++){
			frameVacio->nroFrame=i;
			frameVacio->espacioLibre=tamanioPagina;
			frameVacio->inicio=p;
			frameVacio->datos=list_create();
			list_add(listaDeFrames,frameVacio);
			p+=tamanioPagina;
		}
}

void vaciar_lista_de_frames(t_list *listaDeFrames){
	list_destroy_and_destroy_elements(listaDeFrames,free);
}

void cargar_pcb_a_frame(t_frame *frame, t_pcb *pcb){
	t_dato_en_frame *datopcb=malloc(sizeof(t_dato_en_frame));
	datopcb->tipoContenido= PCB;
	datopcb->pcb=pcb;
	datopcb->tareas=NULL;
	datopcb->tcb=NULL;
	list_add(frame->datos, datopcb);
}

void cargar_tareas_a_frame(t_frame *frame, char *tareas){
	t_dato_en_frame *datotareas=malloc(sizeof(t_dato_en_frame));
	datotareas->tipoContenido= TAREAS;
	datotareas->pcb=NULL;
	datotareas->tareas=tareas;
	datotareas->tcb=NULL;
	list_add(frame->datos, datotareas);
}

void cargar_tcb_a_frame(t_frame *frame, t_tcb *tcb){
	t_dato_en_frame *datotcb=malloc(sizeof(t_dato_en_frame));
	datotcb->tipoContenido=TCB;
	datotcb->pcb=NULL;
	datotcb->tareas=NULL;
	datotcb->tcb=tcb;
	list_add(frame->datos, datotcb);
}

int crear_tabla_de_paginas(t_list *listaDeTablasDePaginas, uint32_t idPatota){
	t_tabla_de_paginas *tablaDePaginas = malloc(sizeof(t_tabla_de_paginas));
	tablaDePaginas->idPatota=idPatota;
	tablaDePaginas->cantPaginas=0;
	tablaDePaginas->paginas=list_create();
	return list_add(listaDeTablasDePaginas,tablaDePaginas);
}

void crear_pagina(t_tabla_de_paginas *tablaDePaginas, t_frame *frame){
	t_pagina *pagina = malloc(sizeof(t_pagina));
	pagina->nroPagina=tablaDePaginas->cantPaginas;
	pagina->bitPresencia=1;
	pagina->bitUso=0;
	pagina->bitModificado=0;
	pagina->frame=frame;
	list_add(tablaDePaginas->paginas, pagina);
	tablaDePaginas->cantPaginas+=1;
}

t_frame *encontrar_frame_vacio(t_list *listaDeFrames, uint32_t tamanioPagina){
	bool _frame_vacio(void *frame){
		return ((t_frame *)frame)->espacioLibre==tamanioPagina;
	}
	t_list *framesVacios=list_filter(listaDeFrames,_frame_vacio);
	return list_get(framesVacios,0);
}

t_frame *encontrar_frame_disponible(t_tabla_de_paginas *tablaDePaginas, t_list *listaDeFrames, uint32_t tamanioPagina, uint32_t tamanioNecesario){
	for(int i=0; i<tablaDePaginas->cantPaginas;i++){
		t_pagina *pagina=list_get(tablaDePaginas->paginas,i);
		t_frame *frame=pagina->frame;
		if(tamanioNecesario<=frame->espacioLibre){
			return frame;
		}
	}
	return NULL;
}

void cargar_pcb_paginacion(t_list *listaDeFrames, t_list *listaDeTablaDePaginas, uint32_t tamanioPagina, uint32_t pid){
	int indiceTabla = crear_tabla_de_paginas(listaDeTablaDePaginas, pid);
	t_tabla_de_paginas *tabla=list_get(listaDeTablaDePaginas, indiceTabla);
	t_frame *frame = encontrar_frame_vacio(listaDeFrames, tamanioPagina);
	void *p=frame->inicio;
	t_pcb *pcb=malloc(sizeof(t_pcb));
	uint32_t *ppid=p;
	(*ppid)=pid;
	pcb->pid=ppid;
	p+=sizeof(uint32_t);
	uint32_t *dirTareas=p;
	(*dirTareas)=0;
	pcb->tareas=dirTareas;
	crear_pagina(tabla, frame);
	cargar_pcb_a_frame(frame, pcb);
}

t_tabla_de_paginas *encontrar_pid_lista_tablas(t_list *listaDetablasDePaginas, uint32_t pid){
	bool _criterio_igual_pid_lista_tablas(void *tabla){
		return ((t_tabla_de_paginas *)tabla)->idPatota==pid;
	}
	return list_find(listaDeTablasDePaginas, _criterio_igual_pid_lista_tablas);
}

void cargar_tareas_paginacion(t_list *listaDeFrames, t_list *listaDeFramesSwap, t_list *listaDeTablasDePaginas, uint32_t tamanioPagina, uint32_t pid, char*tareas){
	t_tabla_de_paginas *tabla = encontrar_pid_lista_tablas(listaDeTablasDePaginas, pid);
	t_frame *frame = encontrar_frame_disponible(tabla, listaDeFrames, tamanioPagina, strlen(tareas));
	if(frame==NULL){
		frame=encontrar_frame_vacio(listaDeFrames, tamanioPagina);
		if(frame!=NULL){
			crear_pagina(tabla, frame);
		}else{
			frame=encontrar_frame_disponible(tabla, listaDeFrames, tamanioPagina, strlen(tareas));
			if(frame==NULL){
				frame=encontrar_frame_vacio(listaDeFramesSwap,tamanioPagina);
				crear_pagina(tabla, frame);
			}
		}
	}
	void*p=frame->inicio+(tamanioPagina-frame->espacioLibre);
	char*t=p;
	strcpy(t,tareas);
	cargar_tareas_a_frame(frame, t);
}

void cargar_tripulante_paginacion(t_list *listaDeFrames, t_list *listaDeFramesSwap, t_list *listaDeTablasDePaginas, uint32_t tamanioPagina, t_tcb *tripulante){
	t_tabla_de_paginas *tabla = encontrar_pid_lista_tablas(listaDeTablasDePaginas, tripulante->pcb);
	t_frame *frame = encontrar_frame_disponible(tabla, listaDeFrames, tamanioPagina, sizeof(tripulante));
	if(frame==NULL){
		frame=encontrar_frame_vacio(listaDeFrames, tamanioPagina);
		if(frame!=NULL){
			crear_pagina(tabla, frame);
		}else{
			frame=encontrar_frame_disponible(tabla, listaDeFrames, tamanioPagina, sizeof(tripulante));
			if(frame==NULL){
				frame=encontrar_frame_vacio(listaDeFramesSwap,tamanioPagina);
				crear_pagina(tabla, frame);
			}
		}
	}
	void*p=frame->inicio+(tamanioPagina-frame->espacioLibre);
	t_tcb *tcb=malloc(sizeof(t_tcb));
	uint32_t *tid=p;
	(*tid)=tripulante->tid;
	tcb->tid=tid;
	p+=sizeof(uint32_t);
	char *estado=p;
	(*estado)=tripulante->estado;
	tcb->estado=estado;
	p+=sizeof(char);
	uint32_t *pos_x=p;
	(*pos_x)=tripulante->pos_x;
	tcb->pos_x;
	p+=sizeof(uint32_t);
	uint32_t *pos_y=p;
	(*pos_y)=tripulante->pos_y;
	tcb->pos_y=pos_y;
	p+=sizeof(uint32_t);
	uint32_t *proximaInstruccion=p;
	(*proximaInstruccion)=0;
	tcb->proxima_instruccion=proximaInstruccion;
	uint32_t *pcb=p;
	(*pcb)=0;
	cargar_tcb_a_frame(frame, tcb);
}

char* tarea_indice(char *tareas, uint32_t indice){
	char *p=strtok(tareas, "\n");
	for(int i=0; i<indice; i++){
		p=strtok(NULL, "\n");
	}
	if(p==NULL){
		return '\0';
	}
	return p;
}

char* proxima_instruccion_tripulante_paginacion(t_list *listaDeTablasDePaginas, uint32_t tid){
	bool _dato_TCB(void *datoTCB){
		return ((t_dato_en_frame *)datoTCB)->tipoContenido == 2;
	}
	bool _dato_TAREAS(void *datoTareas){
		return ((t_dato_en_frame *)datoTareas)->tipoContenido == 1;
	}
	bool _igual_tid_en_dato(void *datotid){
		return ((t_dato_en_frame *)datotid)->tcb->tid==tid;
	}
	bool pagina_objetivo(void* t1) {
		return ((t_pagina*)t1)->bitUso == 1;
	}
	t_pagina* pagina = malloc(sizeof(t_pagina));
	for(int i=0; i<listaDeTablasDePaginas->elements_count; i++){
		t_tabla_de_paginas *tabla=list_get(listaDeTablasDePaginas, i);
		for(int j=0; j<tabla->cantPaginas;j++){
			pagina=list_get(tabla->paginas, j);
			if(pagina->bitPresencia == 0){ //NO ESTÁ CARGADA
				pagina = buscar_pagina(j);
			}

			t_list *datosTCB=list_filter(pagina->frame->datos,_dato_TCB);
			t_dato_en_frame *datoEncontrado = list_find(datosTCB, _igual_tid_en_dato);

			if(datoEncontrado!=NULL){
				for(int k=0; k<tabla->cantPaginas;k++){
					pagina=list_get(tabla->paginas, k);
					if(pagina->bitPresencia == 0){ //NO ESTÁ CARGADA
						pagina = buscar_pagina(k);
					}
				    int hours, minutes, seconds;
				    time_t now;
				    time(&now);
				    struct tm *local = localtime(&now);
				    hours = local->tm_hour;
				    minutes = local->tm_min;
				    seconds = local->tm_sec;
					uint32_t tiempo =  seconds + minutes * 60 + hours * 3600;
					pagina->ultimoUso = tiempo;

					t_dato_en_frame *datoTareas=list_find(pagina->frame->datos,_dato_TAREAS);
					if(datoTareas!=NULL){
						datoEncontrado->tcb->proxima_instruccion+=1;
						free(pagina);
						return tarea_indice(datoTareas->tareas, datoEncontrado->tcb->proxima_instruccion);
					}
				}
			}
		}
	}
	free(pagina);
	return 0;
}

void eliminar_tripulante_paginacion(t_list *listaDeTablasDePaginas, uint32_t tamanioPagina, uint32_t tid){
	bool _dato_TCB(void *datoTCB){
		return ((t_dato_en_frame *)datoTCB)->tipoContenido==TCB;
	}
	bool _igual_tid_en_dato(void *datotid){
		return ((t_dato_en_frame *)datotid)->tcb->tid==tid;
	}
	t_pagina* pagina = malloc(sizeof(t_pagina));
	for(int i=0; i<listaDeTablasDePaginas->elements_count; i++){
		t_tabla_de_paginas *tabla=list_get(listaDeTablasDePaginas, i);
		for(int j=0; j<tabla->cantPaginas;j++){
			pagina=list_get(tabla->paginas, j);
			if(pagina->bitPresencia == 0){ //NO ESTÁ CARGADA
				pagina = buscar_pagina(j);
			}

		    int hours, minutes, seconds;
		    time_t now;
		    time(&now);
		    struct tm *local = localtime(&now);

		    hours = local->tm_hour;
		    minutes = local->tm_min;
		    seconds = local->tm_sec;

			uint32_t tiempo =  seconds + minutes * 60 + hours * 3600;
			pagina->ultimoUso = tiempo;

			t_list *datosTCB=list_filter(pagina->frame->datos,_dato_TCB);
			t_dato_en_frame *datoEncontrado = list_find(datosTCB, _igual_tid_en_dato);
			if(datoEncontrado!=NULL){
				list_remove_by_condition(pagina->frame->datos, _igual_tid_en_dato);
				pagina->frame->espacioLibre-=sizeof(t_tcb);
				if(pagina->frame->espacioLibre==tamanioPagina){
					list_remove(tabla->paginas,i);
				}
				free(pagina);
				break;
			}
		}
	}
	free(pagina);
}

void modificar_posicion_tripulante(t_list *listaDeTablasDePaginas, uint32_t tid, uint32_t pos_x, uint32_t pos_y){
	bool _dato_TCB(void *datoTCB){
		return ((t_dato_en_frame *)datoTCB)->tipoContenido==TCB;
	}
	bool _igual_tid_en_dato(void *datotid){
		return ((t_dato_en_frame *)datotid)->tcb->tid==tid;
	}
	t_pagina* pagina = malloc(sizeof(t_pagina));

	for(int i=0; i<listaDeTablasDePaginas->elements_count; i++){
		t_tabla_de_paginas *tabla=list_get(listaDeTablasDePaginas, i);
		for(int j=0; j<tabla->cantPaginas;i++){
			pagina=list_get(tabla->paginas, i);
			if(pagina->bitPresencia == 0){ //NO ESTÁ CARGADA
				pagina = buscar_pagina(i);
			}

		    int hours, minutes, seconds;
		    time_t now;
		    time(&now);
		    struct tm *local = localtime(&now);

		    hours = local->tm_hour;
		    minutes = local->tm_min;
		    seconds = local->tm_sec;

			uint32_t tiempo =  seconds + minutes * 60 + hours * 3600;
			pagina->ultimoUso = tiempo;

			t_list *datosTCB=list_filter(pagina->frame->datos,_dato_TCB);
			t_dato_en_frame *datoEncontrado = list_find(datosTCB, _igual_tid_en_dato);
			if(datoEncontrado!=NULL){
				datoEncontrado->tcb->pos_x=pos_x;
				datoEncontrado->tcb->pos_y=pos_y;
				free(pagina);
				break;
			}
		}
	}
	free(pagina);
}

void modificar_estado_tripulante(t_list *listaDeTablasDePaginas, uint32_t tid, char estado){
	bool _dato_TCB(void *datoTCB){
		return ((t_dato_en_frame *)datoTCB)->tipoContenido==TCB;
	}
	bool _igual_tid_en_dato(void *datotid){
		return ((t_dato_en_frame *)datotid)->tcb->tid==tid;
	}
	t_pagina* pagina = malloc(sizeof(t_pagina));
	for(int i=0; i<listaDeTablasDePaginas->elements_count; i++){
		t_tabla_de_paginas *tabla=list_get(listaDeTablasDePaginas, i);
		for(int j=0; j<tabla->cantPaginas;i++){
			pagina=list_get(tabla->paginas, i);
			if(pagina->bitPresencia == 0){ //NO ESTÁ CARGADA
				pagina = buscar_pagina(i);
			}

		    int hours, minutes, seconds;
		    time_t now;
		    time(&now);
		    struct tm *local = localtime(&now);

		    hours = local->tm_hour;
		    minutes = local->tm_min;
		    seconds = local->tm_sec;

			uint32_t tiempo =  seconds + minutes * 60 + hours * 3600;
			pagina->ultimoUso = tiempo;

			t_list *datosTCB=list_filter(pagina->frame->datos,_dato_TCB);
			t_dato_en_frame *datoEncontrado = list_find(datosTCB, _igual_tid_en_dato);
			if(datoEncontrado!=NULL){
				datoEncontrado->tcb->estado=estado;
				free(pagina);
				break;
			}
		}
	}
	free(pagina);
}

void listar_tripulantes(t_list *listaDeTablasDePaginas, uint32_t tamanioPagina){
	printf("\n-----------------------------\n");
	for(int i=0; i<listaDeTablasDePaginas->elements_count; i++){
		t_tabla_de_paginas *tabla=list_get(listaDeTablasDePaginas, i);
		for(int j=0; j<tabla->cantPaginas;i++){
			t_pagina *pagina=list_get(tabla->paginas, i);
			if(pagina->frame->espacioLibre==tamanioPagina){
				printf("Marco:%d	Estado:Libre		Proceso:%d		Pagina:%d\n", pagina->frame->nroFrame,tabla->idPatota,pagina->nroPagina);
			}else{
				printf("Marco:%d	Estado:Ocupado		Proceso:%d		Pagina:%d\n", pagina->frame->nroFrame,tabla->idPatota,pagina->nroPagina);
			}
		}
	}
}

bool hay_espacio_disponible(t_list *listaDeFrames, t_list *listaDeFramesSwap, uint32_t tamanioPagina, uint32_t cantTripulantes, uint32_t tareasLen){
	pthread_mutex_lock(&cargar);
	uint32_t tamanioDatos = sizeof(t_pcb)+cantTripulantes*sizeof(t_tcb)+tareasLen;
	int cantPagsNecesarias=1; //CONSIDERAR TAMAÑO DEL SWAP
	while(tamanioPagina*cantPagsNecesarias<tamanioDatos){
		cantPagsNecesarias++;
	}
	bool _frame_vacio(void *frame){
		return ((t_frame *)frame)->espacioLibre==tamanioPagina;
	}
	t_list *framesVacios=list_filter(listaDeFrames,_frame_vacio);
	pthread_mutex_unlock(&cargar);
	if(cantPagsNecesarias<=list_size(framesVacios)){
		return true;
	}
	return false;
}

t_pagina* pagina_a_remover(t_list* paginas_en_memoria){
    void* lru(t_pagina* t1, t_pagina* t2){
        return t1->ultimoUso < t2->ultimoUso ? t1 : t2;
    }

    void* clock(t_pagina* t1, t_pagina* t2){
        return clock_algoritmo(t1) >= clock_algoritmo(t2) ? t1 : t2;
    }

	bool bit_uso_en_uno(void* t1) {
		return ((t_pagina*)t1)->bitUso == 1;
	}

	char* algoritmo = config_get_string_value(config, "ALGORITMO_REEMPLAZO");

    if((strcmp(algoritmo,"LRU") == 0)){
    	t_pagina* objetivo = list_get_minimum(paginas_en_memoria, (void*)lru);
        return objetivo;

    }else{
    	if(list_any_satisfy(paginas_en_memoria,bit_uso_en_uno)){
        	t_pagina* objetivo = list_find(paginas_en_memoria, bit_uso_en_uno);
        	return objetivo;
    	}else{
			t_pagina* objetivo = list_get_maximum(paginas_en_memoria, (void*)clock);
			return objetivo;
    	}
    }
}

uint32_t clock_algoritmo(t_pagina* t){
	if(t->bitUso == 0){
		t->bitUso = 1;
	}
    return t->bitUso;
}

t_pagina* buscar_en_swap(t_pagina* t){
	return t;
}


t_pagina* buscar_swap(int numero){
	t_pagina* pagina = buscar_en_swap(numero);
	t_list* contenido = remover_de_swap(pagina);
	t_pagina* objetivo = pagina_a_remover(lista_en_memoria);
	t_frame* frame = objetivo->frame;

	pagina->frame = objetivo->frame;
	pagina->bitPresencia = 1;
	pagina->bitUso = 0;

	cargar_a_swap(objetivo, frame);
	cargar_a_memoria(contenido, frame);

	return pagina;
}

t_list* remover_de_swap(t_pagina* t){
	t_list* lista=list_create();
	while(contenido_pagina_en_swap){
		void* d = list_get(lista_swap,0);
		list_remove(t->lista,0);
		list_add(lista,d);
	}
	return lista;
}

void cargar_a_memoria(t_list* c, t_frame* f){
	while(list_size(c) != 0){
		t_dato_en_frame* d = list_get(c,0);
		if(d->tipoContenido == 0){
			cargar_pcb_a_frame(f,d);
		}else if(d->tipoContenido == 1){
			cargar_tareas_a_frame(f,d);
		}else{
			cargar_tcb_a_frame(f,d);
		}
	}
	return;
}

void cargar_a_swap(t_pagina* p, t_frame* f){
	while(list_size(f->datos) != 0){
		t_dato_en_frame* d = list_get(f->datos,0);
		if(d->tipoContenido == 0){
			cargar_pcb_a_swap(d);
		}else if(d->tipoContenido == 1){
			cargar_tareas_a_swap(d);
		}else{
			cargar_tcb_a_swap(d);
		}
		list_remove(f->datos,0);
	}
	return;
}

void cargar_pcb_a_swap(t_dato_en_frame* d){
	return;
}

void cargar_tareas_a_swap(t_dato_en_frame* d){
	return;
}

void cargar_tcb_a_swap(t_dato_en_frame* d){
	return;
}
