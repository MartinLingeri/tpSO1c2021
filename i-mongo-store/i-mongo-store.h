#ifndef I-MONGO-STORE_H_
#define I-MONGO-STORE_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<commons/log.h>
#include<commons/bitarray.h>
#include<commons/string.h>
#include<stdint.h>
#include<sys/mman.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<readline/readline.h>
#include<unistd.h>

#include "utils.h"
#include "fscoms.h"

	char* punto_montaje;
	int puerto;
	int tiempo_sincronizacion;
	t_config* config;
	struct stat statbuf;
	int tamanio_bloque;
	int cantidad_bloques;
	t_bitarray *bitmap;
	struct stat bufferParaSuperBloque;
	struct stat buffAgregarRecurso;
	char* pathMongoConfig = "/home/utnso/workspace/tp-2021-1c-Mexico86/i-mongo-store/i-mongo-store.config";
	char* pathBlocks = "/home/utnso/polus/Blocks.ims";



	t_config* leer_config(char*);
	void setear_config();
	void cargar_super_bloque();
	void cargar_bloques();
	void iniciar_file_system();
	void agregar_a_metadata(char*,uint32_t);
	char* realizar_tarea(char*,uint32_t);
	void agregar_recurso(char*,uint32_t);
	void quitar_recurso(char*,uint32_t);
	void crear_bitacora();
	char* obtener_recurso(char*);



#endif /* I-MONGO-STORE_H_ */
