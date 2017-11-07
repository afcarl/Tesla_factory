#include "definitions.h"
#include "main.h"

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
//	if (argc < 4) {
//		printf("Usage: %s <number of cars> <number of spaces> <number of workers>\n", 
//				argv[0]);
//		return EXIT_SUCCESS;
//	}
//	num_cars     = atoi(argv[1]);
//	num_spaces   = atoi(argv[2]);
//	num_workers  = atoi(argv[3]);
//	printf("Job defined, %d workers will build %d cars with %d storage spaces\n",
//			num_workers, num_cars, num_spaces);
	num_cars     = 1;
	num_spaces   = 2;
	num_workers  = 2;
	resource_pack *rpack = (struct resource_pack*) malloc(sizeof(struct resource_pack));
	initResourcePack(rpack, num_spaces, num_workers);

	int i;
	int start = 3;
	int end = start + 1;
	pthread_t workers[8];
	work_pack wpacks[8]; 
	for(i = start; i < end; i++) {
		wpacks[i].tid = i;
		wpacks[i].jid = i;
		wpacks[i].resource = rpack;
		sem_wait(&sem_worker);
		if(pthread_create(&workers[i], NULL, work, &wpacks[i])){
			fprintf(stderr, "error: pthread_create, worker:%d\n", i);
			return EXIT_FAILURE;
		}
	}


	for(i = start; i < end; i++) {
		pthread_join(workers[i], NULL);
	}

	destroySem();
	free(rpack);
	return EXIT_SUCCESS;
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

