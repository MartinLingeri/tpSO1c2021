/*
 * sabotajes.c
 *
 *  Created on: 22 jul. 2021
 *      Author: utnso
 */

#include "sabotajes.h"

void mover_trips(int nuevo_estado){
    pasar_menor_id(trabajando,nuevo_estado);
    pasar_menor_id(listo,nuevo_estado);
}

void desbloquear_trips_inverso(t_list* lista){
    while(list_size(lista) != 0){
        pasar_ultimo(lista, e_listo);
    }
    return;
}

void pasar_menor_id(t_list* lista, int estado_nuevo){
	void* menor_ID(t_tripulante* t1, t_tripulante* t2) {
	    return t1->TID < t2->TID ? t1 : t2;
	}

    while(list_size(lista) != 0){
        t_tripulante* cambio = list_get_minimum(lista, (void*)menor_ID);
        cambiar_estado(cambio->estado, estado_nuevo, cambio);
    }
	return;
}

void pasar_ultimo(t_list* lista, int nuevo){
    int x = list_size(lista);
    t_tripulante* ultimo = list_get(lista,x-1);
    cambiar_estado(ultimo->estado, nuevo, ultimo);
    return;
}

t_tripulante* tripulante_mas_cercano(int x, int y){
    void* t_distancia(t_tripulante* t1, t_tripulante* t2){
        return distancia(t1, x, y) < distancia(t1, x, y) ? t1 : t2;
    }
    t_tripulante* asignado = list_get_minimum(bloqueado_emergencia, (void*)t_distancia);
    return asignado;
}

void resolver_sabotaje(t_tripulante* asignado, t_sabotaje* datos){
    cambiar_estado(asignado->estado, e_bloqueado_emergencia, asignado);
    reportar_bitacora(logs_bitacora(SABOTAJE, " ", " "), asignado->TID, conexion_store);
    int tiempo = atoi(config_get_string_value(config, "DURACION_SABOTAJE"));

	logear(DESPLAZA_TAREA,asignado->TID);
	if(datos->x != asignado->pos_x || datos->y != asignado->pos_y){
		char *x = string_itoa(asignado->pos_x);
		char *y = string_itoa(asignado->pos_y);
		logear_despl(asignado->pos_x, asignado->pos_y, x, y, asignado->TID, conexion_store);
		while(datos->x != asignado->pos_x){
			mover_a(asignado, true, datos->x, config_get_int_value(config, "RETARDO_CICLO_CPU"));
		}
		while(datos->y != asignado->pos_y){
			mover_a(asignado, false, datos->y, config_get_int_value(config, "RETARDO_CICLO_CPU")); //RETARDO CPU
		}
	}
	enviar_desplazamiento_hq(asignado);

	t_buffer* buffer = invocar_fsck(asignado->TID);
	t_paquete* no_paquete = crear_mensaje(buffer, INVOCAR_FSCK);
	logear(LLAMADO_FSCK,0);
	pthread_mutex_lock(&store);
	enviar_paquete(no_paquete, conexion_store);
	pthread_mutex_unlock(&store);
	free(buffer);
	free(no_paquete);

    sleep(tiempo);
    reportar_bitacora(logs_bitacora(SABOTAJE_RESUELTO, " ", " "), asignado->TID, conexion_store);
    logear(SABOTAJE_ATENDIDO,asignado->TID);
    return;
}
