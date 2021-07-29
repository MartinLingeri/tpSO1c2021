#include "discordiador.h"

uint32_t id_ultima_patota = -1;
uint32_t id_ultimo_tripulante = -1;

char* algoritmo;
int grado_multitarea;
int lugar_en_memoria = 1; //INICIAR EN 0 DESPUES

int main(void)
{
	logger = log_create("discordiador.log", "discordiador", true, LOG_LEVEL_INFO);
	logear(INICIO_SISTEMA,0);

	llegada = list_create();
	listo = list_create();
	fin = list_create();
	trabajando = list_create();
	bloqueado_IO = list_create();
	bloqueado_emergencia = list_create();

	setlocale(LC_ALL,"spanish");

	config = config_create("discordiador.config");
	algoritmo = config_get_string_value(config, "ALGORITMO");
	grado_multitarea = config_get_int_value(config, "GRADO_MULTITAREA");

	sem_init(&planif,0,0);
	sem_init(&recibido_hay_lugar,0,1);
	sem_init(&multiprog,0, grado_multitarea);
	sem_init(&listo_para_trabajar,0,0);

	pthread_t hilo_conexion_hq;
	pthread_create(&hilo_conexion_hq, NULL, (void*) conexion_con_hq, NULL);
	pthread_detach((pthread_t) hilo_conexion_hq);

	pthread_t hilo_conexion_store;
	pthread_create(&hilo_conexion_store, NULL, (void*) conexion_con_store, NULL);
	pthread_detach((pthread_t) hilo_conexion_store);

	pthread_t hilo_recibir_conexiones;
	pthread_create(&hilo_recibir_conexiones, NULL, (void*) esperar_conexion, NULL);
	pthread_detach((pthread_t) hilo_recibir_conexiones);

	pthread_t hilo_planificador;
	pthread_create(&hilo_planificador, NULL, (void*) planificador, NULL);
	pthread_detach((pthread_t) hilo_planificador);

	logear(SISTEMA_INICIADO,0);
	leer_consola(logger);
	terminar_programa(conexion_hq, conexion_store, logger, config);
	logear(PROGRAMA_FINALIZADO,0);
	log_destroy(logger);
	return EXIT_SUCCESS;
}

void conexion_con_hq() {
	conexion_hq = crear_conexion(config_get_string_value(config, "IP_MI_RAM_HQ"), config_get_string_value(config, "PUERTO_MI_RAM_HQ"));
}

void conexion_con_store() {
	//conexion_store = crear_conexion(config_get_string_value(config, "IP_I_MONGO_STORE"), config_get_string_value(config, "PUERTO_I_MONGO_STORE"));
}

int recibir_operacion(int socket_cliente)
{
	uint8_t cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(uint8_t), MSG_WAITALL) != 0){
		printf("CODIGO: %d", cod_op);
		puts(" - LLEGADA");
		return cod_op;
	}else{
		close(socket_cliente);
		return -1;
	}
}

void* esperar_conexion() {
	char* ip = config_get_string_value(config, "IP");
	char* puerto = config_get_string_value(config, "PUERTO");
	int server = iniciar_servidor(ip, puerto);

	pthread_mutex_lock(&logs);
	log_info(logger, "Servidor listo para recibir al store");
	pthread_mutex_unlock(&logs);

	int cliente = esperar_cliente(server);
	char* bitacora;
	t_sabotaje* data = malloc(sizeof(t_sabotaje));
	uint32_t lugar;
	t_tarea* tarea = malloc(sizeof(t_tarea));

	while(1){
		int cod_op = recibir_operacion(cliente);
		switch(cod_op){
		case BITACORA:
			bitacora = recibir_bitacora(cliente);

			log_info(logger, bitacora);
			//pthread_mutex_unlock(&logs);
			break;

		case LUGAR_MEMORIA:
			lugar = recibir_hay_lugar(cliente);
			printf("LUGAR: %d\n", lugar);
			sem_post(&recibido_hay_lugar);
			sem_post(&recibido_hay_lugar); //HAY 2 POST XQ EL SEM = -1 ACA
			if(lugar > 0){
				lugar_en_memoria = 1;
			}else{
				lugar_en_memoria = 0;
			}
			break;

		case ALERTA_SABOTAJE:
			logear(SABOTAJE_DETECTADO,0);
			data = recibir_datos_sabotaje(cliente);
			atender_sabotaje(data);
			break;

		case TAREA:
			tarea = recibir_tarea(cliente);
			t_tripulante* objetivo = buscar_tripulante(tarea->TID);
			objetivo->tarea = malloc(tarea->len);
			objetivo->tarea = tarea->tarea_txt;
			sem_post(&objetivo->semaforo);
			sem_post(&objetivo->semaforo);
			break;

		case -1:
			pthread_mutex_unlock(&logs);
			log_error(logger, "Cliente se desconecto. Terminando servidor");
			pthread_mutex_unlock(&logs);
			return 0;

		default:
			break;
		}
	}
	free(ip);
	free(puerto);
	free(bitacora);
	free(tarea);
	free(data);
}

void leer_consola(t_log* logger)
{
	log_info(logger, "Ingrese instrucción por consola. *FIN* para terminar");
	char* leido;
	leido = readline(">");
	while (strcmp(leido, "FIN") != 0) {
		char** instruccion = string_split(leido, " ");
		if(strcmp(instruccion[0], "INICIAR_PATOTA") == 0) {
			if(instruccion[2] != NULL && instruccion[1] != NULL){
				iniciar_patota(instruccion, leido);
			}else{
				logear_error(INST_FALTA_PAR);
			}

		} else if (strcmp(instruccion[0], "LISTAR_TRIPULANTES") == 0) {
			logear(LISTANDO_TRIPULANTES,0);
			listar_tripulantes();

		} else if (strcmp(instruccion[0], "EXPULSAR_TRIPULANTE") == 0) {
			if(instruccion[1] != NULL){
				expulsar_tripulante(instruccion[1]);
			}else{
				logear_error(INST_FALTA_PAR);
			}

		} else if (strcmp(instruccion[0], "INICIAR_PLANIFICACION") == 0) {
			logear(INICIANDO_PLANIF,0);
			sem_post(&planif);

		} else if (strcmp(instruccion[0], "PAUSAR_PLANIFICACION") == 0) {
			logear(PAUSA_PLANIF,0);
			sem_wait(&planif);

		} else if (strcmp(instruccion[0], "OBTENER_BITACORA") == 0) {
			if(instruccion[1] != NULL){
				obtener_bitacora(instruccion[1]);
			}else{
				logear_error(INST_FALTA_PAR);
			}

		} else if (strcmp(instruccion[0], "SIMULAR_SABOTAJE") == 0) {
			//ESTO SIRVE PARA TESTEAR MIENTRAS NO ESTÉ EL IMONGO
			t_sabotaje* data = malloc(sizeof(t_sabotaje));
			data->x = 1;
			data->y = 2;
			atender_sabotaje(data);
			free(data);

		} else if (strcmp(instruccion[0], "SERIALIZAR") == 0) { //PARA TESTEAR
			char* reporte = "Genera oxigeno";
			t_buffer* buffer1 = serializar_reporte_bitacora(2,reporte);
			t_paquete* paquete_cambio_estado1 = crear_mensaje(buffer1, REPORTE_BITACORA);
			pthread_mutex_lock(&hq);
			enviar_paquete(paquete_cambio_estado1, conexion_hq);
			pthread_mutex_unlock(&hq);
			free(buffer1);
			free(paquete_cambio_estado1);

		} else {
			logear_error(INST_NO_RECON);
		}

		leido = readline(">");
	}
	logear(FINALIZANDO_PROGRAMA,0);
	free(leido);
}

void planificador(void* args) {
	int multitarea = config_get_int_value(config, "GRADO_MULTITAREA");

	while(1){
		sem_wait(&listo_para_trabajar);
		while(list_size(listo) > 0 && list_size(trabajando) < multitarea) {
			sem_wait(&planif);
			sem_wait(&multiprog);
			puts("-1 espacio libre");
			if(list_size(listo) != 0){
				t_tripulante* tripulante = (t_tripulante*) list_get(listo, 0);
				cambiar_estado(tripulante->estado, e_trabajando, tripulante);
				sem_post(&tripulante->semaforo);
				sem_post(&tripulante->semaforo);
				sem_post(&planif);
			}
		}
	}
}

void iniciar_patota(char** instruccion, char* leido) {
	//INICIAR_PATOTA 3 /home/utnso/tareas.txt
	//INICIAR_PATOTA 1 home/utnso/tareas.txt
	uint32_t cantidad = atoi(instruccion[1]);
	char* tareas = instruccion[2];

	if(cantidad == 0 || !string_ends_with(tareas,".txt")){
		logear_error(INST_FALTA_PAR);
		return;
	}

	id_ultima_patota++;
	logear(INICIANDO_PATOTA, id_ultima_patota);

	FILE* archivo_tareas =  fopen(tareas, "r");
	char* contenido_tareas = string_new();
	if (archivo_tareas != NULL) {
		char buffer[200];
		while (fgets(buffer, sizeof buffer, archivo_tareas) != NULL) {
			string_append(&contenido_tareas, buffer);
		}
		fclose(archivo_tareas);
	}/*else{
		log_error(logger, "Archivo de tareas no encontrado");
		break;
	}*/

	int longitud = longitud_instr(instruccion);
	uint32_t id_patota = id_ultima_patota;

	t_buffer* buffer = serializar_patota(id_patota, contenido_tareas, cantidad);
	t_paquete* paquete_pcb = crear_mensaje(buffer, PCB_MENSAJE);
	pthread_mutex_lock(&hq);
	enviar_paquete(paquete_pcb, conexion_hq);
	pthread_mutex_unlock(&hq);
	sleep(2);
	free(buffer);
	free(paquete_pcb);

	//sem_wait(&recibido_hay_lugar);
	//sem_wait(&recibido_hay_lugar);

	if(lugar_en_memoria != 0){
		pthread_t hilos[longitud];
		for(int i = 0 ; i<cantidad ; i++) {
			inicializar_tripulante(instruccion, i, longitud, id_patota, hilos[i]);
		}
		logear(PATOTA_INICIADA,id_ultima_patota);
	}else{
		logear_error(NO_MEMORIA);
		id_ultima_patota--;
	}
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

	pthread_mutex_lock(&estados[e_llegada]);
	list_add(llegada, tripulante);
	pthread_mutex_unlock(&estados[e_llegada]);

	t_circular_args* args = malloc(sizeof(t_circular_args));
	args->tripulante = tripulante;
	logear(INICIANDO_TRIPULANTE, tripulante->TID);

	//pedir_tarea(tripulante);
	tripulante->tarea = "EVADIR_IMPUESTOS 12;2;3;3";
	cambiar_estado(tripulante->estado, e_listo, tripulante);

	pthread_create(&hilo, NULL, (void*) circular, args);
	pthread_detach((pthread_t) hilo);
}

void pedir_tarea(t_tripulante* tripulante){
	t_buffer* buffer = serializar_pedir_tarea(tripulante->TID);
	t_paquete* paquete_pedir_tarea = crear_mensaje(buffer, PEDIR_SIGUIENTE_TAREA);
	pthread_mutex_lock(&hq);
	enviar_paquete(paquete_pedir_tarea, conexion_hq);
	pthread_mutex_unlock(&hq);
	free(buffer);
	free(paquete_pedir_tarea);
	logear(SOLICITANDO_TAREA, tripulante->TID);

	sem_wait(&tripulante->semaforo);
	sem_wait(&tripulante->semaforo);
}

void circular(void* args) {
	t_circular_args* argumentos = args;
	iniciar_tripulante_en_hq(argumentos->tripulante);

	sem_init(&argumentos->tripulante->semaforo, 0, 1);
	int tarea_hecha = 0;

//	do{
		//puts("antes pedir");
		//pedir_tarea(argumentos->tripulante);
		//int tarea_hecha = 0;
		//puts("dps pedir");
		while(tarea_hecha != 1 && strcmp(argumentos->tripulante->tarea, "") != 1) {
			sem_wait(&argumentos->tripulante->semaforo);
			sem_wait(&argumentos->tripulante->semaforo);
			tarea_hecha = leer_tarea(argumentos->tripulante, argumentos->tripulante->tarea, config_get_int_value(config, "RETARDO_CICLO_CPU"));
			cambiar_estado(argumentos->tripulante->estado, e_listo, argumentos->tripulante);
		}
//	}while(strcmp(argumentos->tripulante->tarea, "") != 1);

	cambiar_estado(argumentos->tripulante->estado, e_fin, argumentos->tripulante);
	logear(FINALIZA_LISTA_TAREAS,argumentos->tripulante->TID);
	free(argumentos);
	//free(tarea);
}

int leer_tarea(t_tripulante* tripulante, char* tarea, int retardo_ciclo_cpu) {
	logear(COMENZANDO_TAREA,tripulante->TID);
	int quantum_ejec = 0;
    char** parametros_tarea = string_split(tarea, ";");
	char** nombre_tarea = string_split(parametros_tarea[0], " ");
	int pos_x = atoi(parametros_tarea[1]);
	int pos_y = atoi(parametros_tarea[2]);
	int duracion = atoi(parametros_tarea[3]);

	reportar_bitacora(logs_bitacora(INICIO_TAREA, nombre_tarea[0], " "), tripulante->TID, conexion_store);
	logear(DESPLAZA_TAREA,tripulante->TID);
	if(pos_x != tripulante->pos_x || pos_y != tripulante->pos_y){
		logear_despl(tripulante->pos_x, tripulante->pos_y, parametros_tarea[1], parametros_tarea[2], tripulante->TID, conexion_hq);
	}

	if(strcmp(algoritmo,"RR") == 0) {
		while(pos_x != tripulante->pos_x) {
			while(quantum_ejec < quantum && pos_x != tripulante->pos_x){
				quantum_ejec++;
				mover_a(tripulante, true, pos_x, retardo_ciclo_cpu);
			}
			if(quantum_ejec == quantum) {
				logear(FIN_QUANTUM,tripulante->TID);
				cambiar_estado(tripulante->estado, e_listo,tripulante);
				quantum_ejec = 0;
				sem_post(&multiprog);
				sem_wait(&tripulante->semaforo);
				sem_wait(&tripulante->semaforo);
				logear(CONTINUA_TAREA,tripulante->TID);
			}
		}

		while(pos_y != tripulante->pos_y) {
			while(quantum_ejec < quantum && pos_y != tripulante->pos_y){
				quantum_ejec++;
				mover_a(tripulante, false, pos_y, retardo_ciclo_cpu);
			}
			if(quantum_ejec == quantum) {
				logear(FIN_QUANTUM,tripulante->TID);
				cambiar_estado(tripulante->estado, e_listo,tripulante);
				quantum_ejec = 0;
				sem_post(&multiprog);
				sem_wait(&tripulante->semaforo);
				sem_wait(&tripulante->semaforo);
				logear(CONTINUA_TAREA,tripulante->TID);
			}
		}
	}else{
		while(pos_x != tripulante->pos_x){
			mover_a(tripulante, true, pos_x, retardo_ciclo_cpu);
		}
		while(pos_y != tripulante->pos_y){
			mover_a(tripulante, false, pos_y, retardo_ciclo_cpu);
		}
	}
	enviar_desplazamiento_hq(tripulante);

	if(atoi_tarea(nombre_tarea[0]) != -1){
		logear(ESPERA_IO,tripulante->TID);
		cambiar_estado(tripulante->estado, e_bloqueado_IO, tripulante); //TAREAS_IO
		pthread_mutex_lock(&io);
		for(int i = 0; i < duracion; i++) {
			pthread_mutex_lock(&sabotaje);
			sleep(retardo_ciclo_cpu);
			pthread_mutex_unlock(&sabotaje);
		}
		pthread_mutex_unlock(&io);
		cambiar_estado(tripulante->estado, e_listo, tripulante);
	}else{	//TAREAS NO DE IO
		for(int i = 0; i < duracion; i++) {
			sleep(retardo_ciclo_cpu);
			puts("espera 1");
			if(strcmp(algoritmo,"RR") == 0) {
				quantum_ejec++;
				if(quantum_ejec == quantum) {
					logear(FIN_QUANTUM,tripulante->TID);
					cambiar_estado(tripulante->estado, e_listo,tripulante);
					quantum_ejec = 0;
					sem_post(&multiprog);
					sem_wait(&tripulante->semaforo);
					sem_wait(&tripulante->semaforo);
					logear(CONTINUA_TAREA,tripulante->TID);
				}
			}
		}
	}
	realizar_tarea(nombre_tarea[0],duracion,tripulante);
	sem_post(&multiprog);
	puts("+1 espacio libre");
	free(parametros_tarea);
	free(nombre_tarea);
	logear(FINALIZA_TAREA,tripulante->TID);
	return 1;
}

void realizar_tarea(char* tarea, int duracion, t_tripulante* tripulante){
	int tarea_io = atoi_tarea(tarea);

	if(tarea_io != -1){
		t_buffer* buffer = serializar_hacer_tarea(duracion, tarea_io);
		t_paquete* paquete_hacer_tarea = crear_mensaje(buffer, HACER_TAREA);
		pthread_mutex_lock(&store);
		if(conexion_store != -1) {
			printf("va a mandar al store xq socket store: %d\n", conexion_store);
			enviar_paquete(paquete_hacer_tarea, conexion_store);
		} else {
			puts("no se envio a store");
		}
		pthread_mutex_unlock(&store);
		free(buffer);
		free(paquete_hacer_tarea);
	}
	reportar_bitacora(logs_bitacora(FIN_TAREA, tarea, " "), tripulante->TID , conexion_store);
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
    printf("Estado de la nave al día %2d de %2s del año %d a la hora %02d:%02d:%02d \n", day, mes[month], year, hours, minutes, seconds);
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
	if (id == 0){
		logear_error(INST_NO_RECON);
		return;
	};

	if(id < id_ultimo_tripulante+1){
		logear(ELIMINANDO_TRIPULANTE, id);
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
	}else{
		logear_error(TRIP_NO_INICIADO);
	}
	t_buffer* buffer = serializar_eliminar_tripulante(id);
	t_paquete* paquete_r = crear_mensaje(buffer, ELIMINAR_TRIPULANTE);
	pthread_mutex_lock(&hq);
	enviar_paquete(paquete_r, conexion_hq);
	pthread_mutex_unlock(&hq);
	free(buffer);
	free(paquete_r);
	logear(TRIPULANTE_ELIMINADO,id);
	return;
}

void atender_sabotaje(t_sabotaje* datos){
	mover_trips(e_bloqueado_emergencia);
	pthread_mutex_lock(&sabotaje);
	t_tripulante* asignado = tripulante_mas_cercano(datos->x, datos->y);
	logear(COMIENZA_ATENDER_SABOTAJE,asignado->TID);
	resolver_sabotaje(asignado, datos);
	cambiar_estado(asignado->estado, e_listo, asignado);
	desbloquear_trips_inverso(bloqueado_emergencia);
	pthread_mutex_unlock(&sabotaje);
	return;
}

void obtener_bitacora (char* i) {
	int id = atoi(i);
	if(id < 0){
		logear_error(INST_NO_RECON);
		return;
	}
	int ultimo = id_ultimo_tripulante;

	if(id > ultimo || id < 0){
		logear_error(TRIP_NO_INICIADO);
		return;
	}
	logear(BITACORA_SOLICITADA,id);
	t_buffer* buffer = serializar_solicitar_bitacora(id);
	t_paquete* paquete_b = crear_mensaje(buffer, PEDIR_BITACORA);
	pthread_mutex_lock(&store);
	if(conexion_store != -1) {
		printf("va a mandar al store xq socket store: %d\n", conexion_store);
		enviar_paquete(paquete_b, conexion_store);
	} else {
		puts("no se envio a store");
	}

	pthread_mutex_unlock(&store);
	pthread_mutex_lock(&logs);
	log_info(logger, "Bitácora del tripulante N° %d", id);
    free(buffer);
    free(paquete_b);
}
