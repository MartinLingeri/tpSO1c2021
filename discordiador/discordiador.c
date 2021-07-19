#include "discordiador.h"

int conexion_hq = -1;
int conexion_store = -1;
t_config* config;

uint32_t id_ultima_patota = -1;
uint32_t id_ultimo_tripulante = -1;

sem_t recibido_hay_lugar;
sem_t planif;
sem_t multiprog;
sem_t listo_para_trabajar;

int quantum = 0;
char* algoritmo;
int grado_multitarea;
int lugar_en_memoria = 1; //INICIAR EN 0 DESPUES

int main(void)
{
	logger = iniciar_logger();
	logear(INICIO_SISTEMA,0);

	llegada = list_create();
	listo = list_create();
	fin = list_create();
	trabajando = list_create();
	bloqueado_IO = list_create();
	bloqueado_emergencia = list_create();

	setlocale(LC_ALL,"spanish");

	config = leer_config();
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
	sleep(5);
	return EXIT_SUCCESS;
}

void conexion_con_hq() {
	conexion_hq = crear_conexion(config_get_string_value(config, "IP_MI_RAM_HQ"), config_get_string_value(config, "PUERTO_MI_RAM_HQ"));
}

void conexion_con_store() {
	//conexion_store = crear_conexion(config_get_string_value(config, "IP_I_MONGO_STORE"), config_get_string_value(config, "PUERTO_I_MONGO_STORE"));
}

t_log* iniciar_logger(void)
{
	return log_create("discordiador.log", "discordiador", true, LOG_LEVEL_INFO);
}

t_config* leer_config(void)
{
	return config_create("discordiador.config");
}

int esperar_cliente(int socket_servidor)
{
	struct sockaddr_in dir_cliente;
	uint32_t tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	log_info(logger, "Se conecto un cliente!");

	return socket_cliente;
}

int iniciar_servidor(char* ip, char* puerto)
{
	int socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(ip, puerto, &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next)
    {
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            continue;
        }
        break;
    }

	listen(socket_servidor, SOMAXCONN);
    freeaddrinfo(servinfo);

    log_trace(logger, "Listo para escuchar a mi cliente");
    return socket_servidor;
}

int recibir_operacion(int socket_cliente)
{
	uint8_t cod_op;
	puts("antes de recibir codigo");
	if(recv(socket_cliente, &cod_op, sizeof(uint8_t), MSG_WAITALL) != 0){
		printf("CODIGO: %d", cod_op);
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

	/*pthread_mutex_lock(&logs);
	log_info(logger, "Servidor listo para recibir al store");
	pthread_mutex_unlock(&logs);
*/
	int cliente = esperar_cliente(server);
	char* bitacora;
	char* tarea;
	t_sabotaje* data = malloc(sizeof(t_sabotaje));
	uint32_t lugar;

	while(1){
		int cod_op = recibir_operacion(cliente);
		switch(cod_op){
		case BITACORA:
			bitacora = recibir_bitacora(cliente);
			log_info(logger, bitacora);
			pthread_mutex_unlock(&logs);
			break;

		case LUGAR_MEMORIA:
			puts(" - LLEGADA");
			lugar = recibir_hay_lugar(cliente);
			puts("luego de rcv");
			if(lugar == 10){
				puts("LLEGÓ UN 10");
			}
			break;
			/*lugar_en_memoria = recibir_hay_lugar(cliente);
			if(lugar_en_memoria > 0){
				sem_post(&recibido_hay_lugar);
				sem_post(&recibido_hay_lugar); //HAY 2 POST XQ EL SEM = -1 ACA
			}else{
				return(void *)0;
			}
			return (void *)0;*/

		case ALERTA_SABOTAJE:
			logear(SABOTAJE_DETECTADO,0);
			data = recibir_datos_sabotaje(cliente);
			atender_sabotaje(data);
			break;

		case TAREA:
			tarea = recibir_tarea(cliente);
			//ver forma de pasar tarea al hilo del trip
			return tarea;
			break;

		case -1:
			pthread_mutex_unlock(&logs);
			log_error(logger, "I-Mongo-Store se desconecto. Terminando servidor");
			pthread_mutex_unlock(&logs);
			return 0;

		default:
			puts(" OTRO CASO");
			break;
		}
	}
	free(ip);
	free(puerto);
	free(bitacora);
	free(tarea);
	free(data);
}

void terminar_programa(int conexion_hq, int conexion_store, t_log* logger, t_config* config){
	void destruir_tripulante(t_tripulante* t){
		sem_destroy(&(t->semaforo));
	}

	liberar_conexion(conexion_hq);
	liberar_conexion(conexion_store);
	config_destroy(config);
	sem_destroy(&planif);
	sem_destroy(&multiprog);
	sem_destroy(&recibido_hay_lugar);
	sem_destroy(&listo_para_trabajar);
	list_destroy_and_destroy_elements(llegada,(void*)destruir_tripulante);
	list_destroy_and_destroy_elements(listo,(void*)destruir_tripulante);
	list_destroy_and_destroy_elements(fin,(void*)destruir_tripulante);
	list_destroy_and_destroy_elements(trabajando,(void*)destruir_tripulante);
	list_destroy_and_destroy_elements(bloqueado_IO,(void*)destruir_tripulante);
	list_destroy_and_destroy_elements(bloqueado_emergencia,(void*)destruir_tripulante);
	logear(PROGRAMA_FINALIZADO,0);
	log_destroy(logger);
}

void leer_consola(t_log* logger)
{
	log_info(logger, "Ingrese instrucción por consola. Ingrese *FIN* para finalizar Discordiador");
	char* leido;
	leido = readline(">");
	while (strcmp(leido, "FIN") != 0) {
		char** instruccion = string_split(leido, " ");
		if(strcmp(instruccion[0], "INICIAR_PATOTA") == 0) {
			if(instruccion[2] != NULL && instruccion[1] != NULL){
				iniciar_patota(instruccion, leido);
			}else{
				logear(INST_FALTA_PAR,0);
			}

		} else if (strcmp(instruccion[0], "LISTAR_TRIPULANTES") == 0) {
			logear(LISTANDO_TRIPULANTES,0);
			listar_tripulantes();

		} else if (strcmp(instruccion[0], "EXPULSAR_TRIPULANTE") == 0) {
			if(instruccion[1] != NULL){
				expulsar_tripulante(instruccion[1]);
			}else{
				logear(INST_FALTA_PAR,0);
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
				logear(INST_FALTA_PAR,0);
			}

		} else if (strcmp(instruccion[0], "SIMULAR_SABOTAJE") == 0) {
			//ESTO SIRVE PARA TESTEAR MIENTRAS NO ESTÉ EL IMONGO
			t_sabotaje* data = malloc(sizeof(t_sabotaje));
			data->x = 1;
			data->y = 2;
			atender_sabotaje(data);
			free(data);

		} else {
			logear(INST_NO_RECON,0);
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
	puts("iniciar_patota");
	char* tareas = instruccion[2];
	//sem_wait(&recibido_hay_lugar);

	if(cantidad == 0 || !string_ends_with(tareas,".txt")){
		logear(INST_FALTA_PAR,0);
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
	}

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
	if(lugar_en_memoria != 0){
		pthread_t hilos[longitud];
		for(int i = 0 ; i<cantidad ; i++) {
			inicializar_tripulante(instruccion, i, longitud, id_patota, hilos[i]);
		}
		logear(PATOTA_INICIADA,id_ultima_patota);
	}else{
		logear(NO_MEMORIA,0);
		id_ultima_patota--;
	}
}

void iniciar_tripulante_en_hq(t_tripulante* tripulante) {
	t_buffer* buffer = serializar_tripulante(tripulante->TID, tripulante->PID, tripulante->pos_x, tripulante->pos_y, tripulante->estado);
	t_paquete* paquete_tcb = crear_mensaje(buffer, TCB_MENSAJE);
	pthread_mutex_lock(&hq);
	enviar_paquete(paquete_tcb, conexion_hq);
	pthread_mutex_unlock(&hq);
	free(paquete_tcb);
	free(buffer);
}

void enviar_cambio_estado_hq(t_tripulante* tripulante) {
	t_buffer* buffer = serializar_cambio_estado(tripulante->TID, tripulante->estado);
	t_paquete* paquete_cambio_estado = crear_mensaje(buffer, CAMBIO_ESTADO_MENSAJE);
	pthread_mutex_lock(&hq);
	enviar_paquete(paquete_cambio_estado, conexion_hq);
	pthread_mutex_unlock(&hq);
	free(buffer);
	free(paquete_cambio_estado);
}

void enviar_desplazamiento_hq(t_tripulante* tripulante){
	t_buffer* buffer = serializar_desplazamiento(tripulante->TID, tripulante->pos_x, tripulante->pos_y);
	t_paquete* paquete_d = crear_mensaje(buffer, DESPLAZAMIENTO);
	pthread_mutex_lock(&hq);
	enviar_paquete(paquete_d, conexion_hq);
	pthread_mutex_unlock(&hq);
	free(buffer);
	free(paquete_d);
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

	pthread_create(&hilo, NULL, (void*) circular, args);
	pthread_detach((pthread_t) hilo);
}

void circular(void* args) {
	t_circular_args* argumentos = args;
	iniciar_tripulante_en_hq(argumentos->tripulante);

	t_buffer* buffer = serializar_pedir_tarea(argumentos->tripulante->TID);
	t_paquete* paquete_pedir_tarea = crear_mensaje(buffer, PEDIR_SIGUIENTE_TAREA);
	pthread_mutex_lock(&hq);
	enviar_paquete(paquete_pedir_tarea, conexion_hq);
	pthread_mutex_unlock(&hq);
	free(buffer);
	free(paquete_pedir_tarea);

	/*//PEDIR Y RECIBIR PRIMER TAREA
	char* tarea = NULL;
	serializar_pedir_tarea(argumentos->tripulante->TID);*/
	logear(SOLICITANDO_TAREA, argumentos->tripulante->TID);
	char* tarea = "EVADIR_IMPUESTOS 12;2;3;3";
	cambiar_estado(argumentos->tripulante->estado, e_listo, argumentos->tripulante);
	int tarea_hecha = 0;
	sem_init(&argumentos->tripulante->semaforo, 0, 1);

	//	while(strcmp(tarea, "") != 1) {
			while(tarea_hecha != 1) {
				puts("antes semaforo tripulante");
				sem_wait(&argumentos->tripulante->semaforo);
				sem_wait(&argumentos->tripulante->semaforo);
				tarea_hecha = leer_tarea(argumentos->tripulante, tarea, config_get_int_value(config, "RETARDO_CICLO_CPU"));
				cambiar_estado(argumentos->tripulante->estado, e_listo, argumentos->tripulante);
				puts("sale de quantum");
			}
	//	}
	cambiar_estado(argumentos->tripulante->estado, e_fin, argumentos->tripulante);
	logear(FINALIZA_LISTA_TAREAS,argumentos->tripulante->TID);
	puts("Fin tripulante");
	free(argumentos);
	//free(tarea);
}

void cambiar_estado(int estado_anterior, int estado_nuevo, t_tripulante* tripulante) {
   bool es_el_tripulante(void* tripulante_en_lista) {
		return ((t_tripulante*)tripulante_en_lista)->TID == tripulante->TID;
	}
   pthread_mutex_lock(&estados[estado_anterior]);
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
   pthread_mutex_unlock(&estados[estado_anterior]);
   tripulante->estado = estado_nuevo;

   pthread_mutex_lock(&estados[estado_nuevo]);
   switch(estado_nuevo){
    case e_llegada:
        list_add(llegada, tripulante);
        break;
    case e_listo:
        list_add(listo, tripulante);
        sem_post(&listo_para_trabajar);
        printf("+1 Listo para trabajar");
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
   pthread_mutex_unlock(&estados[estado_nuevo]);
   enviar_cambio_estado_hq(tripulante);
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
			puts("circula x");
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
			puts("circula y");
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
		logear(INST_NO_RECON,0);
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
		logear(TRIP_NO_INICIADO,0);
	}
	t_buffer* buffer = serializar_eliminar_tripulante(id);
	t_paquete* paquete_r = crear_mensaje(buffer, ELIMINAR_TRIP);
	pthread_mutex_lock(&hq);
	enviar_paquete(paquete_r, conexion_hq);
	pthread_mutex_unlock(&hq);
	free(buffer);
	free(paquete_r);
	logear(TRIPULANTE_ELIMINADO,id);
	return;
}

void atender_sabotaje(t_sabotaje* datos){
	//CONTROLAR PLANIF. ACTIVADA (O NO, PREGUNTAR)
	log_info(logger,"Iniciando atención de sabotaje");
	mover_trips(e_bloqueado_emergencia);
	pthread_mutex_lock(&sabotaje);
	t_tripulante* asignado = tripulante_mas_cercano(datos->x, datos->y);
	logear(COMIENZA_ATENDER_SABOTAJE,asignado->TID);
	resolver_sabotaje(asignado, datos);
	cambiar_estado(asignado->estado, e_listo, asignado);
	desbloquear_trips_inverso(bloqueado_emergencia);
	pthread_mutex_unlock(&sabotaje);
	log_info(logger,"Sabotaje atendido correctamente");
	return;
}

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

	enviar_paquete(no_paquete, conexion_store);			//ESTO SIRVE PARA TESTEAR MIENTRAS NO ESTÉ EL IMONGO
	t_sabotaje* data = malloc(sizeof(t_sabotaje));
	data->x = 1;
	data->y = 2;
	atender_sabotaje(data);
	pthread_mutex_unlock(&store);
	free(buffer);
	free(no_paquete);
    sleep(tiempo);
    reportar_bitacora(logs_bitacora(SABOTAJE_RESUELTO, " ", " "), asignado->TID, conexion_store);
    logear(SABOTAJE_ATENDIDO,asignado->TID);
    return;
}

void obtener_bitacora (char* i) {
	int id = atoi(i);
	if(id == 0){
		logear(INST_NO_RECON,0);
		return;
	}
	int ultimo = id_ultimo_tripulante;

	if(id > ultimo || id < 0){
		logear(TRIP_NO_INICIADO,0);
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
	log_info(logger, "Bitácora del tripulante N°");//poner el numero
    free(buffer);
    free(paquete_b);
}
