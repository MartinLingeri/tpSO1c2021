#ifndef MI_RAM_HQ_H_
#define MI_RAM_HQ_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>

#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
/*
#include <nivel-gui/nivel-gui.h>
#include <nivel-gui/tad_nivel.h>*/

#include "utils.h"
#include "segmentacion.c"
/*
#define ASSERT_CREATE(nivel, id, err)                                                   \
    if(err) {                                                                           \
        nivel_destruir(nivel);                                                          \
        nivel_gui_terminar();                                                           \
        fprintf(stderr, "Error al crear '%c': %s\n", id, nivel_gui_string_error(err));  \
        return EXIT_FAILURE;                                                            \
}*/
int crear_conexion(char *ip, char* puerto);
t_buffer* serializar_test(uint32_t id);
void eliminar_paquete(t_paquete* paquete);
void enviar_mensaje(char* mensaje, int socket_cliente);
void* serializar_paquete(t_paquete* paquete, int bytes);
#endif /* MI-RAM-HQ_H_ */
