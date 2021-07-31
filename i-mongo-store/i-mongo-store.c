#include "i-mongo-store.h"

int main(void)
{
	logger = log_create("log.log", "I-Mongo-Store", 1, LOG_LEVEL_INFO);

	config = leer_config(pathMongoConfig);
	setear_config();
	iniciar_file_system();

	void iterator(char* value)
	{
		printf("%s\n", value);
	}

	logger = log_create("log.log", "I-Mongo-Store", 1, LOG_LEVEL_DEBUG);

	int server_fd = iniciar_servidor();
	log_info(logger, "Servidor listo para recibir al cliente");
	int cliente_fd = esperar_cliente(server_fd);

	t_list* lista;
	uint32_t tid;
	t_hacer_tarea* tarea = malloc(sizeof(t_hacer_tarea));
	t_rlog* log = malloc(sizeof(t_log));

	int conexion = crear_conexion(config_get_string_value(config, "IP_DS"), config_get_string_value(config, "PUERTO_DS"));
	t_paquete* paquete;
	t_buffer* buffer;

	while(1)
	{
		int cod_op = recibir_operacion(cliente_fd);
		switch(cod_op)
		{
		case REPORTE_BITACORA:
			log = recibir_rbitacora(cliente_fd);
			break;

		case HACER_TAREA:
			tarea = recibir_hacer_tarea(cliente_fd);
			break;

		case PEDIR_BITACORA:
			tid = recibir_pedir_bitacora(cliente_fd);
			buffer=serializar_bitacora("Aca iría la bitacora pedida");
			paquete=crear_mensaje(buffer,BITACORA);
			enviar_paquete(paquete,conexion);
			break;

		case INVOCAR_FSCK:
			tid = recibir_invocar_fsck(cliente_fd);
			break;

		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			return EXIT_FAILURE;

		default:
			log_warning(logger, "Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
	free(tarea);
	free(log);
	close(conexion);

	log_destroy(logger);
	return EXIT_SUCCESS;
}


void setear_config(){
	punto_montaje = config_get_string_value(config,"PUNTO_MONTAJE");
	puerto = config_get_int_value(config,"PUERTO");
	tiempo_sincronizacion = config_get_int_value(config,"TIEMPO_SINCRONIZACION");
	//log_info(logger,"config seteado!");

}

void iniciar_file_system(){
	cargar_super_bloque();
	cargar_bloques();
	realizar_tarea("GENERAR_OXIGENO",4);
	realizar_tarea("GENERAR_OXIGENO",3);
	realizar_tarea("GENERAR_COMIDA",5);

}


void cargar_super_bloque(){

	FILE* arch_sup_bloque;
	char* pathSuperBloque = string_new();
	string_append(&pathSuperBloque,punto_montaje);
	string_append(&pathSuperBloque,"/SuperBloque.ims");

	if(access(pathSuperBloque,F_OK) == 0){

		 log_info(logger,"El archivo del superBloque ya existe");
		 //LEVANTAR EL SUPERBLOQUE EXISTENTE

	}else{

		tamanio_bloque = atoi(readline("Ingrese el tamaño del bloque: "));
		cantidad_bloques = atoi(readline("Ingrese la cantidad de bloques: "));

		log_info(logger,"el tamanio de cada bloque es: %i",tamanio_bloque);
		log_info(logger,"la cantidad de bloques es: %i",cantidad_bloques);

		arch_sup_bloque = fopen("/home/utnso/polus/SuperBloque.ims","w");
		if(arch_sup_bloque == NULL){
			perror("Error al abrir el archivo superBloque \n");
		}

		fwrite(&tamanio_bloque,sizeof(tamanio_bloque),1,arch_sup_bloque);
		fwrite(&cantidad_bloques,sizeof(cantidad_bloques),1,arch_sup_bloque);


				//free(pathSuperBloque);
				fclose(arch_sup_bloque);
	}


}

void cargar_bloques(){
	int fd;
	char* addr;
	off_t off = tamanio_bloque*cantidad_bloques;


	log_info(logger,"comenzando a cargar blocks.ims ...\n");
	fd = open("/home/utnso/polus/Blocks.ims", O_CREAT | O_RDWR,S_IRWXU);

	if(fd == -1) {
	    perror("Error opened file \n");
	    exit(1);
	}

	fstat(fd,&statbuf);
	if(fstat(fd,&statbuf) == -1){
		perror("Error al obtener el tamaño del archivo\n");
		close(fd);
		exit(EXIT_FAILURE);
	}


	if(statbuf.st_size == NULL){

		ftruncate(fd,off);
	}

	fstat(fd,&statbuf);
	if(fstat(fd,&statbuf) == -1){
		perror("Error al obtener el tamaño del archivo\n");
		exit(EXIT_FAILURE);
	}


	addr = mmap(NULL,statbuf.st_size,PROT_WRITE,MAP_SHARED,fd,0);

	if(addr == MAP_FAILED) /* check mapping successful */
	  {
	    perror("Error  mapping \n");
	    close(fd);
	    exit(2);
	  }

	log_info(logger,"El tamanio del archivo creado es de: %i", statbuf.st_size);


}



char* obtener_recurso(char* tarea){

			if(strcmp(tarea,("GENERAR_OXIGENO")) == 0){
				return "oxigeno";

			}else if(strcmp(tarea,("CONSUMIR_OXIGENO")) == 0){
				return "oxigeno";

			}else if(strcmp(tarea,("GENERAR_COMIDA")) == 0){
				return "comida";

			}else if(strcmp(tarea,("CONSUMIR_COMIDA")) == 0){
				return "comida";

			}else if(strcmp(tarea,("GENERAR_BASURA")) == 0){
				return "basura";

			}else if(strcmp(tarea,("DESCARTAR_BASURA")) == 0){
				return "basura";
			}else{
				perror("No se introdujo una tarea disponible");
				exit(EXIT_FAILURE);
			}

}

char* realizar_tarea(char* tarea,uint32_t cant){
		char* caracterDeLlenado;

		if(strcmp(tarea,("GENERAR_OXIGENO")) == 0){
			agregar_recurso("O",cant);
			caracterDeLlenado = "O";

		}else if(strcmp(tarea,("CONSUMIR_OXIGENO")) == 0){
			quitar_recurso("O",cant);
			caracterDeLlenado = "O";

		}else if(strcmp(tarea,("GENERAR_COMIDA")) == 0){
			agregar_recurso("C",cant);
			caracterDeLlenado = "C";

		}else if(strcmp(tarea,("CONSUMIR_COMIDA")) == 0){
			quitar_recurso("C",cant);
			caracterDeLlenado = "C";

		}else if(strcmp(tarea,("GENERAR_BASURA")) == 0){
			agregar_recurso("B",cant);
			caracterDeLlenado = "B";

		}else if(strcmp(tarea,("DESCARTAR_BASURA")) == 0){
			quitar_recurso("B",cant);
			caracterDeLlenado = "B";

	}
		agregar_a_metadata(tarea,cant);

		return caracterDeLlenado;


}

void agregar_recurso(char*recurso,uint32_t cant){
	int fd;
	char* map_blocks;
	char* agregarAArchivo = string_new();
	log_info(logger,"agregando recurso...");



	if(access(pathBlocks,F_OK) == 0){
		//EXISTE EL RECURSO
	fd = open(pathBlocks,O_RDWR,S_IRWXU);
	if(fd == -1) {
		perror("Error opened file \n");
	    exit(1);
	}

	fstat(fd,&buffAgregarRecurso);

	if(fstat(fd,&buffAgregarRecurso) == -1){
		perror("Error al obtener el tamaño del archivo\n");
		close(fd);
		exit(EXIT_FAILURE);
	}

	map_blocks = mmap(NULL,buffAgregarRecurso.st_size+4,PROT_WRITE,MAP_SHARED,fd,0);
		string_append(&agregarAArchivo,map_blocks);
	for(int i=1;i<=cant;i++){
			string_append(&agregarAArchivo,recurso);
	}
		memcpy(map_blocks,agregarAArchivo,strlen(agregarAArchivo));
		int sync = msync(map_blocks,strlen(agregarAArchivo)+1,MS_SYNC);
		if(sync ==-1){
			perror("no se pudo sincronizar");
		}
		int desmapear =munmap(map_blocks,strlen(agregarAArchivo)+1);
		if(desmapear == -1){
			perror("no se pudo desmapear");
		}

		close(fd);
	}else{
		//CREAR ARCHIVO DEL RECURSO
	}


}

void quitar_recurso(char*recurso,uint32_t cant){
	//TODO
}


void agregar_a_metadata(char* tareaARealizar,uint32_t cantidad){
	/*t_config* configMetadata;
	log_info(logger,"agregando a metadata...");
	int fdMetadata;
	int size;
	uint32_t blockCount;
	char* caracter_llenado;
	//uint32_t blocks[];
	//md5
	char* recurso = obtener_recurso(tareaARealizar);
	char* pathMetadata = string_new();
	string_append(&pathMetadata,punto_montaje);
	string_append(&pathMetadata,"/Files/");
	string_append(&pathMetadata,recurso);
	string_append(&pathMetadata,".config");

	configMetadata = leer_config(pathMetadata);


	if(access(pathMetadata,F_OK) == 0){
			//EXISTE EL RECURSO
		fdMetadata = open(pathMetadata,O_RDWR,S_IRWXU);

		if(fdMetadata == -1) {
			perror("Error opened file \n");
		    exit(1);
		}

		size = config_get_int_value(pathMetadata,"SIZE");
		log_info(logger,"el size es: ");
		log_info(logger,"%i",size);

	}else{


	}

*/
}

/*void crear_bitacora(tripulante){
	FILE* bitacoraTripulante;
	uint32_t size;
	//uint32_t blocks[];


}*/


