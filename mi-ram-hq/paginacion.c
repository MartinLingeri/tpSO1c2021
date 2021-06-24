#include<utils.h>

//Cosas que van a ir en algun lado y que ponele funcionan
	t_config*config=leer_config("mi-ram-hq.config");
	int tamanio_memoria = config_get_int_value(config,"TAMANIO_MEMORIA");

	puntero_memoria_principal = malloc(tamanio_memoria);

	esquema_memoria = config_get_string_value(config,"ESQUEMA_MEMORIA");

	tamanio_pagina = config_get_int_value(config,"TAMANIO_PAGINA");

	tamanio_swap = config_get_int_value(config,"TAMANIO_SWAP");

	path_swap = config_get_string_value(config,"PATH_SWAP");

	algoritmo_reemplazo = config_get_string_value(config,"ALGORITMO_REEMPLAZO");

	criterio_seleccion = config_get_string_value(config,"CRITERIO_SELECCION");

	tablasDePaginas=list_create();

	sem_ocupar_frame= malloc(sizeof(sem_t));
	sem_init(sem_ocupar_frame,0,1);

	sem_mutex_eliminar_pagina=malloc(sizeof(sem_t));
	sem_init(sem_mutex_eliminar_pagina,0,1);


	void crear_tabla_de_paginas(t_list *listaDeTablaDePaginas){
		t_list *tablaDePaginas = list_create();
		list_add(listaTablaDePaginas,tablaDePaginas);
	}

	void crear_pagina(t_list *tablaDePaginas, uint32_t nroPagina){
		pagina *pagina = malloc(sizeof(l_pagina));
		pagina->nroPagina= nroPagina;
		pagina->frame=NULL;
		list_add(pagina);
	}

	frame *encontrar_frame_libre(t_list *listaFrames){
		bool _estaLibre(void *elemento){
			return elemento->head->data->libre==TRUE;
		}
		t_list *framesLibres = list_filter(listaFrames, _estaLibre);
		frame* primerFrameLibre = list_get(framesLibres,0);
		return primerFrameLibre;
	}

	void cargar_pagina(frame*frameLibre, pagina* pagina, t_pcb pcb, t_tcb tcb){
		frameLibre->pcb=pcb;
		frameLibre->tareas=pcb->tareas;
		frameLibre->libre=FALSE;
		pagina->frame=frameLibre;
	}

	tabla_de_paginas *encontrar_tabla(t_list *listaDeTablaDePaginas, uint32_t idPatota){
		bool _criterio_igual_idPatota(t_link_element *A){
				return A->head->data->idPatota == idPatota;
		}
		return list_find(listaTablaDePaginas, _criterio_igual_idPatota)
	}

	pagina *encontrar_pagina(t_list *paginas, uint32_t idPagina){
		bool _criterio_igual_nroPagina(t_link_element *A){
				return A->head->data->nroPagina == idPagina;
		}
		return list_find(paginas, _criterio_igual_nro_pagina)
	}

	crear_frames(t_list *listaDeFrames, uint32_t cantFrames){
		listaDeFrames=list_create();
		frame* frame=malloc(sizeof(frame));
		frame->libre=TRUE;
		frame->pcb=NULL;
		frame->tareas=NULL;
		frame->tcb=NULL;
		for(int i=0; i<cantFrames; i++)){
			list_add(listaDeFrames,frame);
		}
		free(frame);
	}

	void iniciar_memoria(uint32_t tamanio_memoria, uint32_t tamanio_pagina){
		uint32_t cantFrames = tamanio_memoria/tamanio_pagina;
		crear_frames(listaDeFrames, cantFrames);
	}

	void vaciar_lista_de_frames(t_list *listaDeFrames){
		list_destroy_and_destroy_elements(lista,free);
	}

	void *encontrar_tripulante(t_list *listaDeFrames, uint32_t tid){
		bool _criterio_igual_tid(void *elemento, uint32_t tid){
			retrun elemento->head->data->tid==tid;
		}
		return list_find(listaDeFrames, _criterio_igual_tid);
	}

	void modificar_posicion_tripulante(t_lsit *listaDeFrames, uint32_t tid, uint32_t pos_x, uint32_t pos_y){
		frame *frameEncontrado = malloc(sizeof(frame));
		frameEncontrado = encontrar_tripulante(listaDeFrames, tid);
		if(frameEncontrado!=NULL){
			frameEncontrado->pos_x=pos_x;
		    frameEncontrado->pos_y=pos_y;
		}
		free(frame);
	}

	void modificar_proxima_tarea_tripulante(t_list *listaDeFrames, uint32_t tid, ){
		frame *frameEncontrado = malloc(sizeof(frame));
		frameEncontrado = encontrar_tripulante(listaDeFrames, tid);
		if(frameEncontrado!=NULL){
			frameEncontrado
		}
	}

	void eliminar_tripulante(uint32_t tid){

	}
