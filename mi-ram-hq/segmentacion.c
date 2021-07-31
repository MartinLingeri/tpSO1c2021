#include "segmentacion.h"


segmento* crear_segmento(int locacion, int tamanio){
	segmento* nuevo_segmento = malloc(sizeof(segmento));
	nuevo_segmento->locacion_inicio = locacion;
	nuevo_segmento->tamanio = tamanio;
	return nuevo_segmento;
}
/*
void* check_for_space(t_list* lista_de_segmentos, int tamanio){
	t_link_element* aux = lista_de_segmentos->head;

	if(aux == NULL){ 			//Si la memoria esta vacia

		return inicio_memoria; 	//Se guarda en el primer lugar de la memoria

	} else {

		while(aux != NULL && ((((segmento*)(aux->next->data))->locacion_inicio)-((((segmento*)(aux->data))->locacion_inicio)+((((segmento*)(aux->data))->tamanio)))) < tamanio){

			aux = aux->next;

		}

		if(aux== NULL){

			printf("No se ha podido encontrar lugar en la memoria. Compactando...");
			compactar(lista_de_segmentos);
			printf("Intentando nuevamente.");
			return NULL;

		} else {

			return (((segmento*)(aux->data))->locacion_inicio) + (((segmento*)(aux->data))->tamanio);

		}
	}
}

void agregar_segmento_FF(t_list* lista_de_segmentos, int tamanio, int locacion){
	int exists = check_for_space(lista_de_segmentos, tamanio);
	//int locacion;
	segmento* nuevo_segmento = crear_segmento(locacion, tamanio);
	t_link_element* aux = lista_de_segmentos->head;
	while(aux->data < nuevo_segmento->locacion_inicio){
		aux = aux->next;
	}
}

void expulsar_tripulante(t_list* lista_de_segmentos){

}

void mostrar_lista_de_segmentos(t_list* lista_de_segmentos){
	if(lista_de_segmentos->head == NULL){
		printf("La lista de segmentos esta vacia.");
	} else {
		printf("Actualmente hay %d segmentos en memoria.\n", lista_de_segmentos->elements_count);
		t_link_element* aux = lista_de_segmentos->head;
		int i = 0;
		while(aux != NULL){
			printf("Segmento: %d\tLocacion: %X\tTamaño: %d bytes\n",i ,((segmento*)aux->data)->locacion_inicio, ((segmento*)aux->data)->tamanio);
			aux = aux->next;
		}
	}
}
*/
void compactar(t_list* lista_de_segmentos){
	/*t_link_element* aux = lista_de_segmentos->head;
	void* siguiente_espacio_libre = inicio_memoria;

	//Si el primer segmento no esta al principio, acomodamos para eso.
	if((((segmento*)(aux->data))->locacion_inicio) != inicio_memoria){
		//Movemos la informacion al principio del bloque.
		memcpy(inicio_memoria, &(((segmento*)(aux->data))->locacion_inicio), sizeof(segmento));

		//Actualizamos la lista para que indique a la locacion 0.
		((segmento*)(aux->data))->locacion_inicio = siguiente_espacio_libre;
	}

	//Para este punto el primer segmento esta en la locacion 0 entonces empezamos a buscar el siguiente.
	siguiente_espacio_libre += (((segmento*)(aux->data))->tamanio);
	aux = aux->next;

	// A partir de aca reiteramos el mismo procedimiento para todos los segmentos siguientes.
	// hasta que no queden mas en la lista.
	while(aux != NULL){
		//Nos fijamos si concuerda la ubicacion libre con el siguiente inicio de segmento.
		if(((segmento*)(aux->data))->locacion_inicio != siguiente_espacio_libre){
			//Si no lo es, acomodamos. Primero memoria, luego en la lista.
			memcpy(siguiente_espacio_libre, &(((segmento*)(aux->data))->locacion_inicio), sizeof(segmento));
			((segmento*)(aux->data))->locacion_inicio = siguiente_espacio_libre;
			//Y pasamos al siguiente
			siguiente_espacio_libre += (((segmento*)(aux->data))->tamanio);
			aux = aux->next;
		} else {
			//Si lo es, avanzamos al siguiente.
			siguiente_espacio_libre += (((segmento*)(aux->data))->tamanio);
			aux = aux->next;
		}
	}*/
	printf("Se ha terminado de compactar.");
}

