#include "discordiador.h"

int conexion_hq = -1;
int conexion_store = -1;
t_log* logger;
t_config* config;

uint32_t id_ultima_patota = -1;
uint32_t id_ultimo_tripulante = -1;

t_list* llegada;
t_list* listo;
t_list* fin;
t_list* trabajando;
t_list* bloqueado_IO;
t_list* bloqueado_emergencia;

bool planificacion_activada = false;

int main(void)
{
	llegada = list_create();
	listo = list_create();
	fin = list_create();
	trabajando = list_create();
	bloqueado_IO = list_create();
	bloqueado_emergencia = list_create();

	setlocale(LC_ALL,"spanish");

	logger = iniciar_logger();
	config = leer_config();


	pthread_t hilo_planificador;
	pthread_create(&hilo_planificador, NULL, (void*) planificador, NULL);
	pthread_detach((pthread_t) hilo_planificador);
	leer_consola(logger);
	terminar_programa(conexion_hq, logger, config);
	sleep(5);
	/*void printear(void* t) {
		printf("el tid es: %d\n", ((t_tripulante*) t)->TID);
	}
	list_iterate(listo, printear);
	puts("en llegada:");
	list_iterate(llegada, printear);*/
	return EXIT_SUCCESS;
}

void conexion_con_hq() {
	while(conexion_hq != 1) {
		conexion_hq = crear_conexion(config_get_string_value(config, "IP_MI_RAM_HQ"), config_get_string_value(config, "PUERTO_MI_RAM_HQ"));
	}
}

void conexion_con_store() {
	while(conexion_store != 1) {
		conexion_store = crear_conexion(config_get_string_value(config, "IP_I_MONGO_STORE"), config_get_string_value(config, "PUERTO_I_MONGO_STORE"));
	}
}

t_log* iniciar_logger(void)
{
	return log_create("discordiador.log", "discordiador", true, LOG_LEVEL_INFO);
}

t_config* leer_config(void)
{
	return config_create("discordiador.config");
}

void leer_consola(t_log* logger)
{
	char* leido;
	leido = readline(">");
	while (strcmp(leido, "") != 0) {
		char** instruccion = string_split(leido, " ");
		if(strcmp(instruccion[0], "INICIAR_PATOTA") == 0) {
			iniciar_patota(instruccion, leido);

		} else if (strcmp(instruccion[0], "LISTAR_TRIPULANTES") == 0) {
			listar_tripulantes();

		} else if (strcmp(instruccion[0], "EXPULSAR_TRIPULANTE") == 0) {

		} else if (strcmp(instruccion[0], "INICIAR_PLANIFICACION") == 0) {
			if(planificacion_activada == false) {
				planificacion_activada = true;
			}
		} else if (strcmp(instruccion[0], "PAUSAR_PLANIFICACION") == 0) {
			if(planificacion_activada == true) {
				planificacion_activada = false;
			}
		} else if (strcmp(instruccion[0], "OBTENER_BITACORA") == 0) {

		} else {
			log_info(logger, "no se reconocio la instruccion");
		}
		leido = readline(">");
	}
	free(leido);
}

void planificador(void* args) {
	while(1) {
		if(planificacion_activada == true && list_size(trabajando) < config_get_int_value(config, "GRADO_MULTITAREA")) {
			if(strcmp(config_get_string_value(config, "ALGORITMO"), "FIFO") == 0) {
				t_tripulante* tripulante = (t_tripulante*) list_get(listo, 0);
				cambiar_estado(tripulante->estado, e_trabajando, tripulante);
				sem_post(&tripulante->semaforo);
			}
		}

	}
}

void terminar_programa(int conexion_hq, t_log* logger, t_config* config)
{
	liberar_conexion(conexion_hq);
	log_destroy(logger);
	config_destroy(config);
}

int longitud_instr(char** instruccion) {
	int largo = 0;
	int i = 0;
	while(instruccion[i] != NULL){
		largo++;
		i++;
	}
	return largo;
}

void iniciar_patota(char** instruccion, char* leido) {
	int cantidad = atoi(instruccion[1]);
	char* tareas = instruccion[2];
	int longitud = longitud_instr(instruccion);
	id_ultima_patota++;
	uint32_t id_patota = id_ultima_patota;
	//t_buffer* buffer = serilizar_patota(id_patota, tareas);
	//t_paquete* paquete_pcb = crear_mensaje(buffer, PCB_MENSAJE);
	//enviar_paquete(paquete_pcb, conexion_hq);
	//enviar paquete pcb y esperar ok de respuesta
	pthread_t hilos[longitud];

	for(int i = 0 ; i<cantidad ; i++) {
		inicializar_tripulante(instruccion, i, longitud, id_patota, hilos[i]);
	}
}

void iniciar_tripulante_en_hq(t_tripulante* tripulante) {
	t_buffer* buffer = serilizar_tripulante(tripulante->TID, tripulante->PID, tripulante->pos_x, tripulante->pos_y, tripulante->estado);
	t_paquete* paquete_tcb = crear_mensaje(buffer, TCB_MENSAJE);
	enviar_paquete(paquete_tcb, conexion_hq);
}

void enviar_cambio_estado_hq(t_tripulante* tripulante) {
	t_buffer* buffer = serilizar_cambio_estado(tripulante->TID, tripulante->estado);
	t_paquete* paquete_cambio_estado = crear_mensaje(buffer, CAMBIO_ESTADO_MENSAJE);
	enviar_paquete(paquete_cambio_estado, conexion_hq);
}

void inicializar_tripulante(char** instruccion, int cantidad_ya_iniciada, int longitud, int id_patota, pthread_t hilo) {
	t_tripulante* tripulante = malloc(sizeof(t_tripulante));
	tripulante->PID = id_patota;
	char** posicion;
	if(instruccion[3] == NULL || 3 + cantidad_ya_iniciada >= longitud) {
		tripulante->pos_x = 0;
		tripulante->pos_y = 0;
	} else {
		 posicion = string_split(instruccion[3+cantidad_ya_iniciada], "|");
		 tripulante->pos_x = atoi(posicion[0]);
		 tripulante->pos_y = atoi(posicion[1]);
	}
	tripulante->estado = e_llegada;
	id_ultimo_tripulante++;
	tripulante->TID = id_ultimo_tripulante;

	sem_t semaforo_tripulante;
	tripulante->semaforo = semaforo_tripulante;

	list_add(llegada, tripulante);

	t_circular_args* args = malloc(sizeof(t_circular_args));
	args->tripulante = tripulante;
	pthread_create(&hilo, NULL, (void*) circular, args);
	pthread_detach((pthread_t) hilo);
}

void circular(void* args) {
	t_circular_args* argumentos = args;

	//iniciar_tripulante_en_hq(argumentos->tripulante);
	//enviar paquete tcb y esperar ok de respuesta
	/*t_buffer* buffer = serilizar_pedir_tarea(argumentos->tripulante->TID);
	t_paquete* paquete_pedir_tarea = crear_mensaje(buffer, PEDIR_SIGUIENTE_TAREA);
	enviar_paquete(paquete_pedir_tarea, conexion_hq);
	//pide 1er tarea
	op_code codigo = recibir_operacion(conexion_hq);
	char* tarea = NULL;
	if(codigo == PEDIR_SIGUIENTE_TAREA) {
		tarea = recibir_tarea(conexion_hq);
	}*/
	//se le retorna la 1er tarea
	char* tarea = "GENERAR_OXIGENO 12;2;3;5";
	cambiar_estado(argumentos->tripulante->estado, e_listo, argumentos->tripulante);

	//PARTE CON BLOQUEOS PORQUE TIENE QUE ESTAR PLANIFICADO
	//INICIAR_PATOTA 3 /home/utnso/tareas/tareasPatota5.txt
	sem_init(&argumentos->tripulante->semaforo, 0, 0);

	while(1) {
		sem_wait(&argumentos->tripulante->semaforo);
		leer_tarea(argumentos->tripulante, tarea, config_get_int_value(config, "RETARDO_CICLO_CPU"));
		//ir a buscar tarea, si se responde con una tarea vacia, tiene q cambiar de estado a fin
		cambiar_estado(argumentos->tripulante->estado, e_listo, argumentos->tripulante);
	}
}

void cambiar_estado(int estado_anterior, int estado_nuevo, t_tripulante* tripulante) {
   bool es_el_tripulante(void* tripulante_en_lista) {
		return ((t_tripulante*)tripulante_en_lista)->TID == tripulante->TID;
	}

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

   tripulante->estado = estado_nuevo;

   switch(estado_nuevo){
    case e_llegada:
        list_add(llegada, tripulante);
        break;
    case e_listo:
        list_add(listo, tripulante);
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
   //enviar_cambio_estado_hq(tripulante);
}

void leer_tarea(t_tripulante* tripulante, char* tarea, int retardo_ciclo_cpu) {

    char** parametros_tarea = string_split(tarea, ";");
	char** nombre_tarea = string_split(parametros_tarea[0], " ");
	int pos_x = atoi(parametros_tarea[1]);
	int pos_y = atoi(parametros_tarea[2]);
	int duracion = atoi(parametros_tarea[3]);

	mover_a(tripulante, true, pos_x, retardo_ciclo_cpu);
	mover_a(tripulante, false, pos_y, retardo_ciclo_cpu);
	//como controlar quantum en caso de RR sleep(atoi(duracion));

	if(strcmp(nombre_tarea[0], "GENERAR_OXIGENO") == 0) {
		//generar_oxigeno(nombre_tarea[1]); //sleep(1) x ser tarea e/s
		puts("genera oxigeno");
	} else if (strcmp(nombre_tarea[0], "CONSUMIR_OXIGENO") == 0) {
		//consumir_oxigeno(nombre_tarea[1]);
	} else if (strcmp(nombre_tarea[0], "GENERAR_COMIDA") == 0) {
		//generar_comida(nombre_tarea[1]);
	} else if (strcmp(nombre_tarea[0], "CONSUMIR_COMIDA") == 0) {
		//consumir_comida(nombre_tarea[1]);
	} else if (strcmp(nombre_tarea[0], "GENERAR_BASURA") == 0) {
		//generar_basura(nombre_tarea[1]);
	} else if (strcmp(nombre_tarea[0], "DESCARTAR_BASURA") == 0) {
		//destruir_basura();
	} else {
		//log_info(logger, "no se reconocio la tarea");
	}
	sleep(duracion * retardo_ciclo_cpu);
	//actualizar_bitacora(tripulante);
}

void listar_tripulantes(){
    void listar(void* t) {
    	char* estados_texto[] = {"Llegada", "Listo", "Fin", "Trabajando", "Bloqueado en I/O", "Bloqueado en emergencia"};
        printf("Tripulante: %3d    Patota: %3d    Estado: %3s \n",
        		((t_tripulante*)t)->TID, ((t_tripulante*)t)->PID, estados_texto[((t_tripulante*)t)->estado]);

	}

    int hours, minutes, seconds, day, month, year;
    time_t now;
    time(&now);

    struct tm *local = localtime(&now);

    hours = local->tm_hour;
    minutes = local->tm_min;
    seconds = local->tm_sec;
    day = local->tm_mday;
    month = local->tm_mon + 1;
    year = local->tm_year + 1900;
    char* mes[] = {" ","Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio", "Julio", "Agosto", "Septiembre", "Octubre", "Noviembre", "Diciembre"};

	printf("---------------------------------------------------------------------------- \n");
    printf("Estado de la nave al día %02d de %02s del año %d a la hora %02d:%02d:%02d \n", day, mes[month], year, hours, minutes, seconds);
    list_iterate(llegada,listar);
    list_iterate(listo,listar);
    list_iterate(trabajando,listar);
    list_iterate(bloqueado_emergencia,listar);
    list_iterate(bloqueado_IO,listar);
    list_iterate(fin,listar);
    printf("---------------------------------------------------------------------------- \n");
}


