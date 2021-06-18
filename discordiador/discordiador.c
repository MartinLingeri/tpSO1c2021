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
pthread_mutex_t bloq;

int quantum = 0;
char* algoritmo;

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
	algoritmo = config_get_string_value(config, "ALGORITMO");

	pthread_t hilo_conexion_hq;
	pthread_create(&hilo_conexion_hq, NULL, (void*) conexion_con_hq, NULL);
	pthread_detach((pthread_t) hilo_conexion_hq);

	pthread_t hilo_conexion_store;
	pthread_create(&hilo_conexion_store, NULL, (void*) conexion_con_store, NULL);
	pthread_detach((pthread_t) hilo_conexion_store);

	pthread_t hilo_planificador;
	pthread_create(&hilo_planificador, NULL, (void*) planificador, NULL);
	pthread_detach((pthread_t) hilo_planificador);
	leer_consola(logger);
	terminar_programa(conexion_hq, conexion_store, logger, config);
	sleep(5);
	return EXIT_SUCCESS;
}

void conexion_con_hq() {
	while(conexion_hq == -1) {
		conexion_hq = crear_conexion(config_get_string_value(config, "IP_MI_RAM_HQ"), config_get_string_value(config, "PUERTO_MI_RAM_HQ"));
		sleep(2);
	}
}

void conexion_con_store() {
	while(conexion_store == -1) {
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
			expulsar_tripulante(instruccion[1]);

		} else if (strcmp(instruccion[0], "INICIAR_PLANIFICACION") == 0) {
			if(planificacion_activada == false) {
				planificacion_activada = true;
			}
		} else if (strcmp(instruccion[0], "PAUSAR_PLANIFICACION") == 0) {
			if(planificacion_activada == true) {
				planificacion_activada = false;
			}
		} else if (strcmp(instruccion[0], "OBTENER_BITACORA") == 0) {

		} else if (strcmp(instruccion[0], "SIMULAR_SABOTAJE") == 0) {
			//ESTO SIRVE PARA TESTEAR MIENTRAS NO ESTÉ EL IMONGO
			t_sabotaje* data = malloc(sizeof(t_sabotaje));
			data->x = 1;
			data->y = 2;
			atender_sabotaje(data);
		} else {
			log_info(logger, "no se reconocio la instruccion");
		}
		leido = readline(">");
	}
	free(leido);
}

void planificador(void* args) {
	quantum = config_get_int_value(config, "QUANTUM");
	while(1) {
		if(planificacion_activada == true && list_size(trabajando) < config_get_int_value(config, "GRADO_MULTITAREA")) {
			t_tripulante* tripulante = (t_tripulante*) list_get(listo, 0);
			cambiar_estado(tripulante->estado, e_trabajando, tripulante);
			sem_post(&tripulante->semaforo);
		}

	}
}

void terminar_programa(int conexion_hq, int conexion_store, t_log* logger, t_config* config)
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
	//INICIAR_PATOTA 3 /home/utnso/tareas.txt
	uint32_t cantidad = atoi(instruccion[1]);
	printf("Cant. trips: %d\n", cantidad);
	char* tareas = instruccion[2];

	FILE* archivo_tareas =  fopen(tareas, "r");
	char* contenido_tareas = string_new();
	if (archivo_tareas != NULL) {
		char buffer[200];
		while (fgets(buffer, sizeof buffer, archivo_tareas) != NULL) {
			string_append(&contenido_tareas, buffer);
		}
		fclose(archivo_tareas);
	}
	int longitud = longitud_instr(instruccion);
	id_ultima_patota++;
	uint32_t id_patota = id_ultima_patota;

	/*while (conexion_hq == -1) {
		sleep(2);
	}*/
	t_buffer* buffer = serilizar_patota(id_patota, contenido_tareas, cantidad);
	t_paquete* paquete_pcb = crear_mensaje(buffer, PCB_MENSAJE);
	enviar_paquete(paquete_pcb, conexion_hq);
	sleep(2);
	free(buffer);
	free(paquete_pcb);

	if(true){//SI HAY LUGAR EN MEMORIA
		pthread_t hilos[longitud];
		for(int i = 0 ; i<cantidad ; i++) {
			inicializar_tripulante(instruccion, i, longitud, id_patota, hilos[i]);
		}
	}else{
		printf("No hay lugar en memoria"); //TRATAR ESTE CASO DE ALGUNA FORMA
	}
}

void iniciar_tripulante_en_hq(t_tripulante* tripulante) {
	pthread_mutex_lock(&bloq);
	/*while (conexion_hq == -1) {
		sleep(2);
	}*/
	t_buffer* buffer = serilizar_tripulante(tripulante->TID, tripulante->PID, tripulante->pos_x, tripulante->pos_y, tripulante->estado);
	t_paquete* paquete_tcb = crear_mensaje(buffer, TCB_MENSAJE);
	enviar_paquete(paquete_tcb, conexion_hq);
	free(paquete_tcb);
	free(buffer);
	pthread_mutex_unlock(&bloq);
}

void enviar_cambio_estado_hq(t_tripulante* tripulante) {
	while (conexion_hq == -1) {
		sleep(2);
	}
	t_buffer* buffer = serilizar_cambio_estado(tripulante->TID, tripulante->estado);
	t_paquete* paquete_cambio_estado = crear_mensaje(buffer, CAMBIO_ESTADO_MENSAJE);
	enviar_paquete(paquete_cambio_estado, conexion_hq);
	free(buffer);
	free(paquete_cambio_estado);
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
	iniciar_tripulante_en_hq(argumentos->tripulante);
	/*while (conexion_hq == -1) {
		sleep(2);
	}*/

	t_buffer* buffer = serilizar_pedir_tarea(argumentos->tripulante->TID);
	t_paquete* paquete_pedir_tarea = crear_mensaje(buffer, PEDIR_SIGUIENTE_TAREA);
	enviar_paquete(paquete_pedir_tarea, conexion_hq);

	//PEDIR Y RECIBIR PRIMER TAREA

	/*
	op_code codigo = recibir_operacion(conexion_hq);
	char* tarea = NULL;
	if(codigo == PEDIR_SIGUIENTE_TAREA) {
		tarea = recibir_tarea(conexion_hq);
	}
	*/

	char* tarea = "GENERAR_OXIGENO 12;2;3;5";
	cambiar_estado(argumentos->tripulante->estado, e_listo, argumentos->tripulante);

	//PARTE CON BLOQUEOS PORQUE TIENE QUE ESTAR PLANIFICADO
	//INICIAR_PATOTA 3 /home/utnso/tareas/tareasPatota5.txt
	sem_init(&argumentos->tripulante->semaforo, 0, 0);

	while(1) {
		sem_wait(&argumentos->tripulante->semaforo);
		leer_tarea(argumentos->tripulante, tarea, config_get_int_value(config, "RETARDO_CICLO_CPU"));
		//BUSCAR SIG TAREA
		//SI VACIO SE PASA A TERMINADO
		cambiar_estado(argumentos->tripulante->estado, e_listo, argumentos->tripulante);
	}
	free(argumentos);
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
   enviar_cambio_estado_hq(tripulante);
}

void leer_tarea(t_tripulante* tripulante, char* tarea, int retardo_ciclo_cpu) {
	int quantum_ejec = 0;
    char** parametros_tarea = string_split(tarea, ";");
	char** nombre_tarea = string_split(parametros_tarea[0], " ");
	int pos_x = atoi(parametros_tarea[1]);
	int pos_y = atoi(parametros_tarea[2]);
	int duracion = atoi(parametros_tarea[3]);

	reportar_bitacora(logs_bitacora(INICIO_TAREA, nombre_tarea[0], " "), &tripulante->TID);
	if(pos_x != tripulante->pos_x || pos_y != tripulante->pos_y){
		logear_despl(tripulante->pos_x, tripulante->pos_y, parametros_tarea[1], parametros_tarea[2], tripulante->TID, conexion_hq);
	}

	if(strcmp(algoritmo,"RR")) {
		while(pos_x != tripulante->pos_x && quantum_ejec < atoi(quantum)) {
			quantum_ejec++;
			mover_a(tripulante, true, pos_x, retardo_ciclo_cpu);
		}
		if(quantum_ejec == atoi(quantum)) {
			cambiar_estado(tripulante->estado, e_listo, tripulante);
			sem_wait(&tripulante->semaforo);
		}
		while(pos_y != tripulante->pos_y && quantum_ejec < atoi(quantum)) {
			quantum_ejec++;
			mover_a(tripulante, false, pos_y, retardo_ciclo_cpu);
		}
		if(quantum_ejec == quantum) {
			cambiar_estado(tripulante->estado, e_listo, tripulante);
			sem_wait(&tripulante->semaforo);
		}
	}else{
		while(pos_x != tripulante->pos_x){
			mover_a(tripulante, true, pos_x, retardo_ciclo_cpu);
		}
		while(pos_y != tripulante->pos_y){
			mover_a(tripulante, false, pos_y, retardo_ciclo_cpu);
		}
	}

	for(int i = 0; i < duracion; i++) {
		sleep(retardo_ciclo_cpu);
		if(strcmp(algoritmo,"RR")) {
			quantum_ejec++;
			if(quantum_ejec == quantum) {
				cambiar_estado(tripulante->estado, e_listo, tripulante);
				sem_wait(&tripulante->semaforo);
			}
		}
	}

	if(strcmp(nombre_tarea[0], "GENERAR_OXIGENO") == 0) {
		generar_oxigeno(nombre_tarea[1], tripulante->TID); //TODAS ESTAS FUNCIONES SON BASICAMENTE LA MISMA, cuando lo pensas el switch este no es necesario
		puts("genera oxigeno");
	} else if (strcmp(nombre_tarea[0], "CONSUMIR_OXIGENO") == 0) {
		consumir_oxigeno(nombre_tarea[1], tripulante->TID);
	} else if (strcmp(nombre_tarea[0], "GENERAR_COMIDA") == 0) {
		generar_comida(nombre_tarea[1], tripulante->TID);
	} else if (strcmp(nombre_tarea[0], "CONSUMIR_COMIDA") == 0) {
		consumir_comida(nombre_tarea[1], tripulante->TID);
	} else if (strcmp(nombre_tarea[0], "GENERAR_BASURA") == 0) {
		generar_basura(nombre_tarea[1], tripulante->TID);
	} else if (strcmp(nombre_tarea[0], "DESCARTAR_BASURA") == 0) {
		destruir_basura(nombre_tarea[1], tripulante->TID);
	} else {
		t_buffer* buffer = serilizar_hacer_tarea(duracion, nombre_tarea[0], tripulante->TID);
		t_paquete* paquete_hacer_tarea = crear_mensaje(buffer, HACER_TAREA);
		enviar_paquete(paquete_hacer_tarea, conexion_hq);
		free(buffer);
		free(paquete_hacer_tarea);
	}
	reportar_bitacora(logs_bitacora(FIN_TAREA, nombre_tarea[0], " "), tripulante->TID);
}

void listar_tripulantes(){
    void listar(void* t) {
    	char* estados_texto[] = {"Llegada", "Listo", "Fin", "Trabajando", "Bloqueado en I/O", "Bloqueado en emergencia"};
        printf("Tripulante: %3d    Patota: %3d    Estado: %3s    Posición: (%d,%d) \n",
        		((t_tripulante*)t)->TID, ((t_tripulante*)t)->PID, estados_texto[((t_tripulante*)t)->estado],
				((t_tripulante*)t)->pos_x, ((t_tripulante*)t)->pos_y);

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

void expulsar_tripulante(char* i) {
	int id = atoi(i);

	bool es_el_tripulante(void* tripulante_en_lista) {
		return ((t_tripulante*)tripulante_en_lista)->TID == id;
	}

	if(list_any_satisfy(llegada, es_el_tripulante)){
    	t_tripulante* tripulante = list_find(llegada,es_el_tripulante);
        cambiar_estado(e_llegada, e_fin, tripulante);

    }else if(list_any_satisfy(listo, es_el_tripulante)){
    	t_tripulante* tripulante = list_find(listo,es_el_tripulante);
        cambiar_estado(e_listo, e_fin, tripulante);

    }else if(list_any_satisfy(bloqueado_IO, es_el_tripulante)){
    	t_tripulante* tripulante = list_find(bloqueado_IO,es_el_tripulante);
        cambiar_estado(e_bloqueado_IO, e_fin, tripulante);

    }else if(list_any_satisfy(bloqueado_emergencia, es_el_tripulante)){
    	t_tripulante* tripulante = list_find(bloqueado_emergencia,es_el_tripulante);
        cambiar_estado(e_bloqueado_emergencia, e_fin, tripulante);

    }else{
    	t_tripulante* tripulante = list_find(trabajando,es_el_tripulante);
        cambiar_estado(e_trabajando, e_fin, tripulante);
    }
   //enviar_remover_a_hq(id);
   return;
}

void atender_sabotaje(t_sabotaje* datos){
    //HILO O ALGO QUE ESPERE SABOTAJE SIN ESPERA ACTIVA Y LLAME A ESTO
	//CONTROLAR PLANIF. ACTIVADA
	puts("Atendiendo sabotaje...");
   mover_trips(e_bloqueado_emergencia);
   t_tripulante* asignado = tripulante_mas_cercano(datos->x, datos->y);
   resolver_sabotaje(asignado, datos);
   cambiar_estado(asignado->estado, e_listo, asignado);
   //VER TEMA EXACTO DEL ORDEN
   desbloquear_trips_inverso(bloqueado_emergencia);
   puts("Sabotaje atendido...");
   return;
}

void mover_trips(int nuevo_estado){
    //MUTEX PARA CADA LISTA
    pasar_menor_id(trabajando,nuevo_estado);
    pasar_menor_id(listo,nuevo_estado);
    while(list_size(bloqueado_IO) != 0){
        pasar_menor_id(bloqueado_IO, nuevo_estado);
        pasar_menor_id(listo, nuevo_estado);
    }
}

void desbloquear_trips_inverso(t_list* lista){
    while(list_size(lista) != 0){
        pasar_ultimo(lista, e_listo);
    }
    return;
}

void pasar_menor_id(t_list* lista, int estado_nuevo){
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

static void* menor_ID(t_tripulante* t1, t_tripulante* t2) {
    return t1->TID < t2->TID ? t1 : t2;
}

double distancia(t_tripulante* trip, int x, int y){
    return ((x - trip->pos_x)*(x - trip->pos_x) + (y - trip->pos_y)*(y - trip->pos_y));
    //NO ANDA EL sqrt()
    //return sqrt((x - trip->pos_x)*(x - trip->pos_x) + (y - trip->pos_y)*(y - trip->pos_y));
}

void resolver_sabotaje(t_tripulante* asignado, t_sabotaje* datos){
    cambiar_estado(asignado->estado, e_bloqueado_emergencia, asignado);
    reportar_bitacora(logs_bitacora(SABOTAJE, " ", " "), asignado->TID);
    int tiempo = atoi(config_get_string_value(config, "DURACION_SABOTAJE"));

	if(datos->x != asignado->pos_x || datos->y != asignado->pos_y){
		char *x = malloc(sizeof(asignado->pos_x));
		char *y = malloc(sizeof(asignado->pos_y));
		string_itoa(asignado->pos_x, x, 10);
		string_itoa(asignado->pos_y, y, 10);
		logear_despl(asignado->pos_x, asignado->pos_y, x, y, asignado->TID, conexion_store);
		while(datos->x != asignado->pos_x){
			mover_a(asignado, true, datos->x, config_get_int_value(config, "RETARDO_CICLO_CPU"));
		}
		while(datos->y != asignado->pos_y){
			mover_a(asignado, false, datos->y, config_get_int_value(config, "RETARDO_CICLO_CPU")); //RETARDO CPU
		}
	}

    //invocar_FSCK_de_hq(); //ESTO ES UNA SERIALIZACION Y ESPERAR RTA ANTES DE SEGUIR
    sleep(tiempo); //SUPONGO Q ACÁ NO HAY FIFO NI RR
    reportar_bitacora(logs_bitacora(SABOTAJE_RESUELTO, " ", " "), asignado->TID);
    return;
}

