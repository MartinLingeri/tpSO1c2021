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
	t_frame *frame;
}t_pagina;

typedef enum{
	PCB,
	TAREAS,
	TCB
}tipo_contenido;

typedef struct{
	tipo_contenido tipoContenido;
	t_pcb *pcb;
	char *tareas;
	t_tcb *tcb;
}t_dato_en_frame;

t_list *listaDeTablasDePaginas;
t_list *listaDeFrames;

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
void eliminar_tripulante_paginacion(t_list*, uint32_t);
void modificar_posicion_tripulante(t_list*, uint32_t, uint32_t, uint32_t);
void modificar_estado_tripulante(t_list*, uint32_t, char);
void listar_tripulantes(t_list*, uint32_t);
bool hay_espacio_disponible(t_list*, uint32_t, uint32_t, uint32_t);

