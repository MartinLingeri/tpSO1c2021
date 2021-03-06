#ifndef SERIALIZACIONES_H_
#define SERIALIZACIONES_H_

#include "utils.h"

t_buffer* serializar_patota(uint32_t id, char* tareas, uint32_t trips); //DEVOLVER SI HAY LUGAR
t_buffer* serializar_tripulante(uint32_t id, uint32_t pid, uint32_t pos_x, uint32_t pos_y, uint32_t estado);
t_buffer* serializar_cambio_estado(uint32_t id, uint32_t estado);
t_buffer* serializar_pedir_tarea(uint32_t id); //DEVOLVER TAREA O NADA
t_buffer* serializar_reporte_bitacora(uint32_t id, char* reporte);
t_buffer* serializar_desplazamiento(uint32_t tid, uint32_t x_nuevo, uint32_t y_nuevo);
t_buffer* serializar_hacer_tarea(uint32_t cantidad, int tarea);
t_buffer* serializar_eliminar_tripulante(uint32_t id);
t_buffer* serializar_solicitar_bitacora(uint32_t id);
t_buffer* invocar_fsck(uint32_t id);

t_sabotaje* recibir_datos_sabotaje(int socket_cliente);
uint32_t recibir_hay_lugar(int socket_cliente);
char* recibir_bitacora(int socket_cliente);
t_tarea* recibir_tarea(int socket_cliente);
void recibir_cambio_estado(int socket_cliente);

#endif /* SERIALIZACIONES_H_ */
