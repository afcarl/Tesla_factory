#include "definitions.h"
#include "main.h"
#include <omp.h>

sem_t sem_worker;
sem_t sem_space;

sem_t sem_skeleton;
sem_t sem_engine;
sem_t sem_chassis;
sem_t sem_body;

sem_t sem_window;
sem_t sem_tire;
sem_t sem_battery;
sem_t sem_car;

int num_cars;
int num_spaces;
int num_workers;

int main(int argc, char** argv)
{
	if (argc < 4) {
		printf("Usage: %s <number of cars> <number of spaces> <number of workers>\n", 
				argv[0]);
		return EXIT_SUCCESS;
	}
	num_cars     = atoi(argv[1]);
	num_spaces   = atoi(argv[2]);
	num_workers  = atoi(argv[3]);
	printf("Job defined, %d workers will build %d cars with %d storage spaces\n",
			num_workers, num_cars, num_spaces);
	resource_pack *rpack = (struct resource_pack*) malloc(sizeof(struct resource_pack));
	initResourcePack(rpack, num_spaces, num_workers);

	int i;
	int num_car_made = 0; 
	pthread_t *workers = malloc(num_workers * sizeof(pthread_t));
	work_pack *wpacks = malloc(num_workers * sizeof(work_pack)); 
	double production_time = omp_get_wtime();
	int parts_cnt[8];
	for (i = 0; i < 8; i++){
		parts_cnt[i] = num_cars;
	}
	int job_order[8]; 
	if(num_workers < 3) {
		job_order[0] = 3;
		job_order[4] = 7;
		for(i = 0; i < 3; i++) job_order[i+1] = i;
		for(i = 4; i < 7; i++) job_order[i+1] = i;
	}
	else {
		job_order[0] = 3;
		job_order[1] = 7;
		for(i = 0; i < 3; i++) job_order[i+2] = i;
		for(i = 4; i < 7; i++) job_order[i+1] = i;
	}

	for(i = 0; i < 8; i++) {
		printf("job_order[%d] = %d\n", i, job_order[i]);
	}


	int *workerId = malloc(sizeof(int));
	while (num_car_made < num_cars) {
		for(i = 0; i < 8 ; i++) {
			int j;
			int parts = 0;
			for (j = 0; j < 8; j++){
				parts += parts_cnt[j];
			}
			if( parts == 0) {
				break;
			}
			if(parts_cnt[job_order[i]] > 0) {
				sem_wait(&sem_worker);
				sem_getvalue(&sem_worker, workerId);
				wpacks[i].tid = *workerId;
				wpacks[i].jid = job_order[i];
				wpacks[i].resource = rpack;
				parts_cnt[job_order[i]] -= 1;
				if(job_order[i] == WINDOW) wpacks[i].times = 7; 
				else if(job_order[i] == TIRE) wpacks[i].times = 4; 
				else wpacks[i].times = 1;
				if(pthread_create(&workers[i], NULL, work, &wpacks[i])) {
					fprintf(stderr, "error: pthread_create, worker:%d\n", i);
					exit(EXIT_FAILURE);
				}
			}
		}
		sem_getvalue(&sem_car, &num_car_made);
	}	
	production_time = omp_get_wtime() - production_time;

	reportResults(production_time);

	destroySem();
	free(rpack);
	return EXIT_SUCCESS;
}

void createWorker(int* parts_cnt, pthread_t *workers, work_pack *wpacks, resource_pack *rpack, int i){
	int *workerId = malloc(sizeof(int));
	printf("i = %d\n", i);
	if(parts_cnt[i] > 0) {
		sem_getvalue(&sem_worker, workerId);
		wpacks[i].tid = *workerId;
		wpacks[i].jid = i;
		wpacks[i].resource = rpack;
		if((i % 8) == WINDOW) wpacks[i].times = 7;
		else if((i % 8) == TIRE) wpacks[i].times = 4;
		else wpacks[i].times = 1;
		parts_cnt[i]--;
		if(pthread_create(&workers[i], NULL, work, &wpacks[i])) {
			fprintf(stderr, "error: pthread_create, worker:%d\n", i);
			exit(EXIT_FAILURE);
		}
	}
}

void reportResults(double production_time) {
	int *sem_value = malloc(sizeof(int));
	printf("=====Final report=====\n");

	sem_getvalue(&sem_skeleton, sem_value);
	printf("Unused Skeleton: %d\n",   *sem_value);
	sem_getvalue(&sem_engine,   sem_value);
	printf("Unused Engine: %d\n",     *sem_value);
	sem_getvalue(&sem_chassis,  sem_value);
	printf("Unused Chassis: %d\n",    *sem_value);
	sem_getvalue(&sem_body,     sem_value);
	printf("Unused Body: %d\n",       *sem_value);
	sem_getvalue(&sem_window,   sem_value);
	printf("Unused Window: %d\n",     *sem_value);
	sem_getvalue(&sem_tire,     sem_value);
	printf("Unused Tire: %d\n",       *sem_value);
	sem_getvalue(&sem_battery,  sem_value);
	printf("Unused Battery: %d\n",    *sem_value);

	sem_getvalue(&sem_space, sem_value);
	if (*sem_value < num_spaces) {
		printf("There are waste car parts!\n");
	}
	sem_getvalue(&sem_car, sem_value);
	printf("Production of %d %s done, production time: %f sec, space usage: %d\n", 
			*sem_value,
			*sem_value > 1 ? "cars" : "car",	       
			production_time, num_spaces);
	printf("==========\n");
	free(sem_value);
}

void initResourcePack(struct resource_pack *pack,
		int space_limit, int num_workers) {
	initSem();
	pack->space_limit  = space_limit;
	pack->num_workers  = num_workers;
	pack->sem_space    = &sem_space   ;
	pack->sem_worker   = &sem_worker  ;

	pack->sem_skeleton = &sem_skeleton;
	pack->sem_engine   = &sem_engine  ;
	pack->sem_chassis  = &sem_chassis ;
	pack->sem_body     = &sem_body    ;

	pack->sem_window   = &sem_window  ;
	pack->sem_tire     = &sem_tire    ;
	pack->sem_battery  = &sem_battery ;
	pack->sem_car      = &sem_car     ;
}

int destroySem(){
#if DEBUG
	printf("Destroying semaphores...\n");
#endif
	sem_destroy(&sem_worker);
	sem_destroy(&sem_space);

	sem_destroy(&sem_skeleton);
	sem_destroy(&sem_engine);
	sem_destroy(&sem_chassis);
	sem_destroy(&sem_body);

	sem_destroy(&sem_window);
	sem_destroy(&sem_tire);
	sem_destroy(&sem_battery);
	sem_destroy(&sem_car);
#if DEBUG
	printf("Semaphores destroyed\n");
#endif
	return 0;
}

int initSem(){
#if DEBUG
	printf("Initiating semaphores...\n");
#endif
	sem_init(&sem_worker,   0, num_workers);
	sem_init(&sem_space,    0, num_spaces);

	sem_init(&sem_skeleton, 0, 0);
	sem_init(&sem_engine,   0, 0);
	sem_init(&sem_chassis,  0, 0);
	sem_init(&sem_body,     0, 0);

	sem_init(&sem_window,   0, 0);
	sem_init(&sem_tire,     0, 0);
	sem_init(&sem_battery,  0, 0);
	sem_init(&sem_car,      0, 0);
#if DEBUG
	printf("Init semaphores done!\n");
#endif
	return 0;
}
