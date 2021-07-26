/*
 * sabotajes.h
 *
 *  Created on: 22 jul. 2021
 *      Author: utnso
 */

#ifndef SABOTAJES_H_
#define SABOTAJES_H_

#include "utils.h"
#include "serializaciones.h"
#include "logs.h"
#include "cambio_estado.h"

void mover_trips(int nuevo_estado);
void desbloquear_trips_inverso(t_list* lista);
void pasar_menor_id(t_list* lista, int estado_nuevo);
void pasar_ultimo(t_list* lista, int nuevo);
t_tripulante* tripulante_mas_cercano(int x, int y);
void resolver_sabotaje(t_tripulante* asignado, t_sabotaje* datos);

#endif /* SABOTAJES_H_ */
