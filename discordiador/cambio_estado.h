/*
 * cambio_estado.h
 *
 *  Created on: 26 jul. 2021
 *      Author: utnso
 */

#ifndef CAMBIO_ESTADO_H_
#define CAMBIO_ESTADO_H_

#include "utils.h"
#include "serializaciones.h"
#include "logs.h"

void iniciar_tripulante_en_hq(t_tripulante* tripulante);
void enviar_cambio_estado_hq(t_tripulante* tripulante);
void enviar_desplazamiento_hq(t_tripulante* tripulante);
void cambiar_estado(int estado_anterior, int estado_nuevo, t_tripulante* tripulante);

#endif /* CAMBIO_ESTADO_H_ */
