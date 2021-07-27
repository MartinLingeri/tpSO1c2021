/*
 * cambio_estado.c
 *
 *  Created on: 26 jul. 2021
 *      Author: utnso
 */

#include "cambio_estado.h"

void cambiar_estado(int estado_anterior, int estado_nuevo, t_tripulante* tripulante) {
   bool es_el_tripulante(void* tripulante_en_lista) {
		return ((t_tripulante*)tripulante_en_lista)->TID == tripulante->TID;
	}
   pthread_mutex_lock(&estados[estado_anterior]);
   switch(estado_anterior){
    case e_llegada:
        list_remove_by_condition(llegada, es_el_tripulante);
        break;
    case e_listo:
        list_remove_by_condition(listo, es_el_tripulante);
        break;
    case e_fin:
        list_remove_by_condition(fin, es_el_tripulante);
        break;
    case e_trabajando:
        list_remove_by_condition(trabajando, es_el_tripulante);
        break;
    case e_bloqueado_IO:
        list_remove_by_condition(bloqueado_IO, es_el_tripulante);
        break;
    case e_bloqueado_emergencia:
        list_remove_by_condition(bloqueado_emergencia, es_el_tripulante);
        break;
   }
   pthread_mutex_unlock(&estados[estado_anterior]);
   tripulante->estado = estado_nuevo;

   pthread_mutex_lock(&estados[estado_nuevo]);
   switch(estado_nuevo){
    case e_llegada:
        list_add(llegada, tripulante);
        break;
    case e_listo:
        list_add(listo, tripulante);
        sem_post(&listo_para_trabajar);
        printf("Pasa a listo el tripulante: %d\n", tripulante->TID);
        break;
    case e_fin:
        list_add(fin, tripulante);
        break;
    case e_trabajando:
        list_add(trabajando, tripulante);
        break;
    case e_bloqueado_IO:
        list_add(bloqueado_IO, tripulante);
        break;
    case e_bloqueado_emergencia:
        list_add(bloqueado_emergencia, tripulante);
        break;
    }
   pthread_mutex_unlock(&estados[estado_nuevo]);
   enviar_cambio_estado_hq(tripulante);
}


void iniciar_tripulante_en_hq(t_tripulante* tripulante) {
	t_buffer* buffer = serializar_tripulante(tripulante->TID, tripulante->PID, tripulante->pos_x, tripulante->pos_y, tripulante->estado);
	t_paquete* paquete_tcb = crear_mensaje(buffer, TCB_MENSAJE);
	pthread_mutex_lock(&hq);
	enviar_paquete(paquete_tcb, conexion_hq);
	pthread_mutex_unlock(&hq);
	free(paquete_tcb);
	free(buffer);
}

void enviar_cambio_estado_hq(t_tripulante* tripulante) {
	t_buffer* buffer = serializar_cambio_estado(tripulante->TID, tripulante->estado);
	t_paquete* paquete_cambio_estado = crear_mensaje(buffer, CAMBIO_ESTADO_MENSAJE);
	pthread_mutex_lock(&hq);
	enviar_paquete(paquete_cambio_estado, conexion_hq);
	pthread_mutex_unlock(&hq);
	free(buffer);
	free(paquete_cambio_estado);
}

void enviar_desplazamiento_hq(t_tripulante* tripulante){
	t_buffer* buffer = serializar_desplazamiento(tripulante->TID, tripulante->pos_x, tripulante->pos_y);
	t_paquete* paquete_d = crear_mensaje(buffer, DESPLAZAMIENTO);
	pthread_mutex_lock(&hq);
	enviar_paquete(paquete_d, conexion_hq);
	pthread_mutex_unlock(&hq);
	free(buffer);
	free(paquete_d);
}
