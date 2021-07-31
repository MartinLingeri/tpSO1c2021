/*
 * pags.c
 *
 *  Created on: 28 jul. 2021
 *      Author: utnso
 */

#include "pags.h"

void crear_lista_de_frames(void *memoria, uint32_t tamanioMemoria){
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

void crear_lista_de_frames_swap(void *memoriaSwap, uint32_t tamanioSwap){
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
	datopcb->tipoContenido = PCB;
	datopcb->pcb=pcb;
	datopcb->tareas=NULL;
	datopcb->tcb=NULL;
	list_add(frame->datos, datopcb);
}

void cargar_tareas_a_frame(t_frame *frame, char *tareas){
	t_dato_en_frame *datotareas=malloc(sizeof(t_dato_en_frame));
	datotareas->tipoContenido = TAREAS;
	datotareas->pcb=NULL;
	datotareas->tareas=tareas;
	datotareas->tcb=NULL;
	list_add(frame->datos, datotareas);
}

void cargar_tcb_a_frame(t_frame *frame, t_tcb *tcb){
	t_dato_en_frame *datotcb=malloc(sizeof(t_dato_en_frame));
	datotcb->tipoContenido = TCB;
	datotcb->pcb=NULL;
	datotcb->tareas=NULL;
	datotcb->tcb=tcb;
	list_add(frame->datos, datotcb);
}

int crear_tabla_de_paginas(uint32_t idPatota){
	t_tabla_de_paginas *tablaDePaginas = malloc(sizeof(t_tabla_de_paginas));
	tablaDePaginas->idPatota=idPatota;
	tablaDePaginas->cantPaginas=0;
	tablaDePaginas->paginas=list_create();
	return list_add(listaDeTablasDePaginas,tablaDePaginas);
}

void crear_pagina(t_tabla_de_paginas *tablaDePaginas, t_frame *frame){
	puts("3.4");
	t_pagina *pagina = malloc(sizeof(t_pagina));
	pagina->nroPagina=nroPagGlobal;
	pagina->bitPresencia=1;
	pagina->bitUso=0;
	pagina->bitModificado=0;
	pagina->frame=frame;
	puts("3.5");
	list_add(tablaDePaginas->paginas, pagina);
	puts("3.6");
	tablaDePaginas->cantPaginas+=1;
}

t_frame *encontrar_frame_vacio(){
	bool _frame_vacio(void *frame){
		return ((t_frame *)frame)->espacioLibre==tamanioPagina;
	}
	t_list *framesVacios=list_filter(listaDeFrames,_frame_vacio);
	if(framesVacios==NULL){
		t_list*pagsEnMemoria=paginas_en_memoria();
		t_pagina*paginaARemover=pagina_a_remover(pagsEnMemoria);
		cargar_a_swap(paginaARemover,paginaARemover->frame);
		return paginaARemover->frame;
	}
	return list_get(framesVacios,0);
}

t_frame *encontrar_frame_disponible(t_tabla_de_paginas *tablaDePaginas, uint32_t tamanioNecesario){
	puts("2.1");
	for(int i=0; i<tablaDePaginas->cantPaginas;i++){
		puts("2.2");
		t_pagina *pagina=list_get(tablaDePaginas->paginas,i);
		puts("2.3");
		t_frame *frame=pagina->frame;
		if(tamanioNecesario<=frame->espacioLibre){
			puts("2.5");
			return frame;
		}
		puts("2.4");
	}
	puts("2.5");
	return NULL;
}

void cargar_pcb_paginacion(t_iniciar_patota* patota){
	puts("Guardando patota");
	t_buffer* buffer = serializar_hay_lugar_memoria(1);
	t_paquete* paquete = crear_mensaje(buffer,1);
	enviar_paquete(paquete,conexion);

	int indiceTabla = crear_tabla_de_paginas(patota->pid);
	t_tabla_de_paginas *tabla=list_get(listaDeTablasDePaginas, indiceTabla);
	t_frame *frame = encontrar_frame_vacio();
	void *p=frame->inicio;
	t_pcb *pcb=malloc(sizeof(t_pcb));
	uint32_t *ppid=p;
	(*ppid)=patota->pid;
	pcb->pid= patota->pid;

	p+=sizeof(uint32_t);
	uint32_t *dirTareas=p;
	(*dirTareas)=0;
	pcb->tareas= patota->tareas;

	nroPagGlobal+=1;
	crear_pagina(tabla, frame);
	cargar_pcb_a_frame(frame, pcb);
	logear(LLEGA_PCB,patota->pid);
	puts("Patota guardada");
	cargar_tareas_paginacion(patota->pid,patota->tareas);
}

t_tabla_de_paginas *encontrar_pid_lista_tablas(uint32_t pid){
	bool _criterio_igual_pid_lista_tablas(void *tabla){
		return ((t_tabla_de_paginas *)tabla)->idPatota==pid;
	}
	return list_find(listaDeTablasDePaginas, _criterio_igual_pid_lista_tablas);
}

void cargar_tareas_paginacion(uint32_t pid, char*tareas){
	puts("Guardando tareas");
	t_tabla_de_paginas *tabla = encontrar_pid_lista_tablas(pid);
	t_frame *frame = encontrar_frame_disponible(tabla, strlen(tareas));
	if(frame==NULL){
		frame=encontrar_frame_vacio();
		nroPagGlobal+=1;
		crear_pagina(tabla, frame);
	}
	void*p=frame->inicio+(tamanioPagina-frame->espacioLibre);
	char*t=p;
	strcpy(t,tareas);
	cargar_tareas_a_frame(frame, t);
	puts("tareas guardadas");
}

void cargar_tripulante_paginacion(t_tcb *tripulante){
	puts("Guardando tripulante");

	printf("PID: %d\n", tripulante->pcb);
	t_tabla_de_paginas *tabla = encontrar_pid_lista_tablas(tripulante->pcb);
	t_frame *frame = encontrar_frame_disponible(tabla, sizeof(tripulante));
	if(frame==NULL){
		frame=encontrar_frame_vacio();
		nroPagGlobal+=1;
		crear_pagina(tabla, frame);
	}
	void *p = frame->inicio+(tamanioPagina-frame->espacioLibre);
	t_tcb *tcb = malloc(sizeof(t_tcb));

	uint32_t *tid = p;
	(*tid) = tripulante->tid;
	tcb->tid = tripulante->tid;
	p+=sizeof(uint32_t);
    printf("TID: %d \n", tcb->tid);

	char *estado=p;
	(*estado)=tripulante->estado;
	tcb->estado= tripulante->estado;
	p+=sizeof(char);
    printf("ESTADO: %c \n", tcb->estado);

	uint32_t *pos_x=p;
	(*pos_x)=tripulante->pos_x;
	tcb->pos_x = tripulante->pos_x;
	p+=sizeof(uint32_t);
    printf("X: %d \n", tcb->pos_x);

	uint32_t *pos_y=p;
	(*pos_y)=tripulante->pos_y;
	tcb->pos_y= tripulante->pos_y;
	p+=sizeof(uint32_t);
    printf("Y: %d \n", tcb->pos_y);

	uint32_t *proximaInstruccion=p;
	(*proximaInstruccion)=0;
	tcb->proxima_instruccion=0;
	uint32_t *pcb=p;
	(*pcb)=0;

	cargar_tcb_a_frame(frame, tcb);
	puts("Tripulante guardado");
	logear(LLEGA_TCB,tripulante->tid);
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

void proxima_instruccion_tripulante_paginacion(uint32_t tid){
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
			t_list *datosTCB=list_filter(pagina->frame->datos,_dato_TCB);
			t_dato_en_frame *datoEncontrado = list_find(datosTCB, _igual_tid_en_dato);

			if(datoEncontrado!=NULL){
				if(pagina->bitPresencia == 0){ //NO ESTÁ CARGADA
					pagina = buscar_pagina(j);
				}

				for(int k=0; k<tabla->cantPaginas;k++){
					pagina=list_get(tabla->paginas, k);
				    time_t now;
				    time(&now);
				    struct tm *local = localtime(&now);
					uint32_t tiempo =  local->tm_sec + local->tm_min * 60 + local->tm_hour * 3600;
					pagina->ultimoUso = tiempo;

					t_dato_en_frame *datoTareas=list_find(pagina->frame->datos,_dato_TAREAS);
					if(datoTareas!=NULL){
						if(pagina->bitPresencia == 0){ //NO ESTÁ CARGADA
							pagina = buscar_pagina(k);
						}
						datoEncontrado->tcb->proxima_instruccion+=1;
						pagina->ultimoUso = tiempo;
						free(pagina);
						char* proximaTarea = tarea_indice(datoTareas->tareas, datoEncontrado->tcb->proxima_instruccion);
						t_buffer* buffer=serializar_tarea(tid,proximaTarea);
						t_paquete* paquete=crear_mensaje(buffer,3);
						enviar_paquete(paquete,conexion);
						logear(SOLICITA_TAREA,tid);
					}else{
						t_buffer* buffer=serializar_tarea(tid,"");
						t_paquete* paquete=crear_mensaje(buffer,3);
						enviar_paquete(paquete,conexion);
						logear(SOLICITA_TAREA,tid);
					}
				}
			}
		}
	}
	free(pagina);
	return;
}

void eliminar_tripulante_paginacion(uint32_t tid){
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
		    time_t now;
		    time(&now);
		    struct tm *local = localtime(&now);

			uint32_t tiempo =  local->tm_sec + local->tm_min * 60 + local->tm_hour * 3600;
			pagina->ultimoUso = tiempo;

			t_list *datosTCB=list_filter(pagina->frame->datos,_dato_TCB);
			t_dato_en_frame *datoEncontrado = list_find(datosTCB, _igual_tid_en_dato);
			if(datoEncontrado!=NULL){
				if(pagina->bitPresencia == 0){ //NO ESTÁ CARGADA
					pagina = buscar_pagina(i);
				}
				list_remove_by_condition(pagina->frame->datos, _igual_tid_en_dato);
				pagina->frame->espacioLibre-=sizeof(t_tcb);
				if(pagina->frame->espacioLibre==tamanioPagina){
					list_remove(tabla->paginas,i);
				}
				logear(ELIMINAR_TRIP,tid);
				free(pagina);
				break;
			}
		}
	}
	free(pagina);
}

void modificar_posicion_tripulante(t_tcb* tripulante){
	bool _dato_TCB(void *datoTCB){
		return ((t_dato_en_frame *)datoTCB)->tipoContenido==TCB;
	}
	bool _igual_tid_en_dato(void *datotid){
		return ((t_dato_en_frame *)datotid)->tcb->tid==tripulante->tid;
	}
	t_pagina* pagina = malloc(sizeof(t_pagina));

	for(int i=0; i<listaDeTablasDePaginas->elements_count; i++){
		t_tabla_de_paginas *tabla=list_get(listaDeTablasDePaginas, i);
		for(int j=0; j<tabla->cantPaginas;i++){
			pagina=list_get(tabla->paginas, i);
		    time_t now;
		    time(&now);
		    struct tm *local = localtime(&now);
			uint32_t tiempo =  local->tm_sec + local->tm_min * 60 + local->tm_hour * 3600;
			pagina->ultimoUso = tiempo;

			t_list *datosTCB=list_filter(pagina->frame->datos,_dato_TCB);
			t_dato_en_frame *datoEncontrado = list_find(datosTCB, _igual_tid_en_dato);
			if(datoEncontrado!=NULL){
				if(pagina->bitPresencia == 0){ //NO ESTÁ CARGADA
					pagina = buscar_pagina(i);
				}
				datoEncontrado->tcb->pos_x=tripulante->pos_x;
				datoEncontrado->tcb->pos_y=tripulante->pos_y;
				free(pagina);
				break;
			}
		}
	}
	free(pagina);
}

void modificar_estado_tripulante(t_tcb* tripulante){
	bool _dato_TCB(void *datoTCB){
		return ((t_dato_en_frame *)datoTCB)->tipoContenido==TCB;
	}
	bool _igual_tid_en_dato(void *datotid){
		return ((t_dato_en_frame *)datotid)->tcb->tid==tripulante->tid;
	}

	puts("inicia modificar estado");
	t_pagina* pagina = malloc(sizeof(t_pagina));
	for(int i=0; i<listaDeTablasDePaginas->elements_count; i++){
		t_tabla_de_paginas *tabla = list_get(listaDeTablasDePaginas, i);
		for(int j=0; j<tabla->cantPaginas;j++){
			pagina=list_get(tabla->paginas, j);
			t_list *datosTCB=list_filter(pagina->frame->datos,_dato_TCB);
			t_dato_en_frame *datoEncontrado = list_find(datosTCB, _igual_tid_en_dato);

		    time_t now;
			time(&now);
			struct tm *local = localtime(&now);
			uint32_t tiempo =  local->tm_sec + local->tm_min * 60 + local->tm_hour * 3600;
			pagina->ultimoUso = tiempo;

			if(datoEncontrado!=NULL){
				if(pagina->bitPresencia == 0){ //NO ESTÁ CARGADA
					pagina = buscar_pagina(j);
				}
				datoEncontrado->tcb->estado=tripulante->estado;
				free(pagina);
				puts("fin modificar estado");
				break;
			}
		}
	}
	puts("fin modificar estado");
	free(pagina);
}

void dump_memoria(){ //NO TESTEADO
	int tamanioPagina = config_get_int_value(config, "TAMANIO_MEMORIA");
	char* esquema_memoria = config_get_string_value(config, "ESQUEMA_MEMORIA");
	char* path = string_new();
	char* tiempo = temporal_get_string_time("%H:%M:%S");

	string_append(&path,"home/"); //ESTO NO ANDA EN LA VM, VER COMO HACER
	string_append(&path,"Dump_");
	string_append(&path,tiempo);
	string_append(&path,".dmp");

    FILE* dump = fopen(path,"w");

	pthread_mutex_lock(&cargar);
	if(strcmp(esquema_memoria, "PAGINACION") == 0){
		puts("antes for tabla pags");
		if(list_size(listaDeTablasDePaginas) == 0){
			fprintf(dump,"No hay tablas páginas iniciadas");
			return;
		}
		for(int i=0; i<listaDeTablasDePaginas->elements_count; i++){
			t_tabla_de_paginas *tabla=list_get(listaDeTablasDePaginas, i);
			fprintf(dump,"\n-----------------------------\n");
			puts("antes for pags");

			if(list_size(tabla->cantPaginas) == 0){
				fprintf(dump,"No hay páginas iniciadas");
				return;
			}
			for(int j=0; j<tabla->cantPaginas;j++){
				puts("antes list get paginas");
				t_pagina *pagina=list_get(tabla->paginas, j);
				if(pagina->frame->espacioLibre==tamanioPagina){
					fprintf(dump,"Marco:%2d Estado:%2s Proceso:-		Pagina:-\n", pagina->frame->nroFrame, "Libre");
				}else{
					fprintf(dump,"Marco:%2d	Estado:%2s Proceso:%2d Pagina:%2d \n", pagina->frame->nroFrame,"Ocupado", tabla->idPatota,pagina->nroPagina);
				}
			}
		}

	}else{
		//DUMP SEGMENTACION
	}
	pthread_mutex_unlock(&cargar);

	fclose(dump);
	free(path);
	free(tiempo);
}

bool hay_espacio_disponible(uint32_t cantTripulantes, uint32_t tareasLen){
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
	t_list *framesVaciosSwap=list_filter(listaDeFramesSwap,_frame_vacio);
	pthread_mutex_unlock(&cargar);
	if(cantPagsNecesarias<=list_size(framesVacios)+list_size(framesVaciosSwap)){
		return true;
	}
	return false;
}

t_list* paginas_en_memoria(){
	bool _esta_en_memoria(void*elemento){
		return ((t_pagina *)elemento)->bitPresencia==1;
	}
	t_list* paginasEnMemoria=list_create();
	for(int i=0; i<listaDeTablasDePaginas->elements_count;i++){
		t_tabla_de_paginas*tabla=list_get(listaDeTablasDePaginas,i);
		list_add_all(paginasEnMemoria,list_filter(tabla->paginas,_esta_en_memoria));
	}
	return paginasEnMemoria;
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

t_pagina* buscar_pagina(int numero){
	t_pagina* pagina = buscar_en_swap(numero);
	t_list* contenido = remover_de_swap(pagina);
	t_pagina* objetivo = pagina_a_remover(paginas_en_memoria());
	t_frame* frame = objetivo->frame;

	pagina->frame = objetivo->frame;
	pagina->bitPresencia = 1;
	pagina->bitUso = 0;

	logear(A_SWAP,objetivo->nroPagina);
	cargar_a_swap(objetivo, frame);
	//list_remove(lista_en_memoria,objetivo);
	logear(A_MEMORIA,pagina->nroPagina);
	cargar_a_memoria(contenido, frame);

	return pagina;
}

t_pagina* buscar_en_swap(t_pagina* t){
	return t;
}

t_list* remover_de_swap(t_pagina* t){
	t_list* lista=list_create();
	/*while(contenido_pagina_en_swap){
		void* d = list_get(lista_swap,0);
		list_remove(t->lista,0);
		list_add(lista,d);
	}*/
	return lista;
}

void cargar_a_memoria(t_list* c, t_frame* f){
	while(list_size(c) != 0){
		t_dato_en_frame* d = list_get(c,0);
		if(d->tipoContenido == 0){
			cargar_pcb_a_frame(f,d->pcb);
		}else if(d->tipoContenido == 1){
			cargar_tareas_a_frame(f,d->tareas);
		}else{
			cargar_tcb_a_frame(f,d->tcb);
		}
	}
	return;
}

void cargar_a_swap(t_pagina* p, t_frame* f){
	while(list_size(f->datos) != 0){
		t_dato_en_frame* d = list_get(f->datos,0);
		if(d->tipoContenido == PCB){
			cargar_pcb_a_swap(f,d->pcb);
		}else if(d->tipoContenido == TAREAS){
			cargar_tareas_a_swap(f,d->tareas);
		}else{
			cargar_tcb_a_swap(f,d->tcb);
		}
		list_remove(f->datos,0);
	}
	return;
}

void cargar_pcb_a_swap(t_frame *frame, t_pcb *pcb){
	return;
}

void cargar_tareas_a_swap(t_frame *frame, char *d){
	return;
}

void cargar_tcb_a_swap(t_frame *frame, t_tcb *d){
	return;
}
