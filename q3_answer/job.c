#include "job.h"

void reportJobDone(sem_t *sem_worker, int num_worker) {
	int *num_free_worker = malloc(sizeof(num_free_worker));
	sem_getvalue(sem_worker, num_free_worker);
	if(*num_free_worker < num_worker) {
		free(num_free_worker);
		sem_post(sem_worker);
	}else {
		printf("Error, number of free workers exceeds num_worker\n");
		free(num_free_worker);
		exit(1);
	}
}

int requestSpace(sem_t *space) {
#if DEBUG
	int *num_free_space = malloc(sizeof(num_free_space));
	sem_getvalue(space, num_free_space);
	printf("Requesting free space, current space=%d...\n", *num_free_space);
#endif
	sem_wait(space);
#if DEBUG
	sem_getvalue(space, num_free_space);
	printf("Space requested, current space=%d...\n", *num_free_space);
#endif

	return 0;
}

void releaseSpace(sem_t *space, int space_limit) {
	int *num_free_space = malloc(sizeof(num_free_space));
	sem_getvalue(space, num_free_space);
	if(*num_free_space < space_limit) {
#if DEBUG
		printf("releasing free space, current space=%d...\n", *num_free_space);
#endif
		sem_post(space);
#if DEBUG
		sem_getvalue(space, num_free_space);
		printf("Space released, current space=%d...\n", *num_free_space);
#endif
		free(num_free_space);
	} else {
		printf("Error, releasing space that doesn't exist\n");
		free(num_free_space);
		exit(1);
	}
}

void makeItem(sem_t *space, int makeTime, sem_t* item) {
	sleep(makeTime);
	requestSpace(space);
	sem_post(item);
}

int getItem(sem_t *space, int space_limit, sem_t *item) {
	int result = sem_trywait(item);
	if(result == 0) releaseSpace(space, space_limit);
	return result; 
}

void makeSkeleton(sem_t *sem_space, sem_t *sem_skeleton) {
	makeItem(sem_space, TIME_SKELETON, sem_skeleton);
}

void makeEngine(sem_t *sem_space, sem_t *sem_engine) {
	makeItem(sem_space, TIME_ENGINE, sem_engine);
}

void makeChassis(sem_t *sem_space, sem_t *sem_chassis) {
	makeItem(sem_space, TIME_CHASSIS, sem_chassis);
}

void makeWindow(sem_t *sem_space, sem_t *sem_window) {
	makeItem(sem_space, TIME_WINDOW, sem_window);
}

void makeTire(sem_t *sem_space, sem_t *sem_tire) {
	makeItem(sem_space, TIME_TIRE, sem_tire);
}

void makeBattery(sem_t *sem_space, sem_t *sem_battery ) {
	// call makeItem and pass in the time for makeing battery
	makeItem(sem_space, TIME_BATTERY, sem_battery);
}

void makeBody(sem_t *sem_space, int space_limit, sem_t *sem_body,
		sem_t *sem_skeleton, sem_t *sem_engine, sem_t *sem_chassis) {
	int num_ske, num_eng, num_cha;
	num_ske = 0; num_eng = 0; num_cha = 0;
	while(num_ske < 1 || num_eng < 1 || num_cha < 1) {	
		if(num_ske < 1 && getItem(sem_space, space_limit, sem_skeleton) == 0) num_ske++;
		if(num_eng < 1 && getItem(sem_space, space_limit, sem_engine)   == 0) num_eng++;
		if(num_cha < 1 && getItem(sem_space, space_limit, sem_chassis)  == 0) num_cha++;
	}
	makeItem(sem_space, TIME_BODY, sem_body);
}

void makeCar(sem_t *sem_space, int space_limit, sem_t *sem_car,
		sem_t *sem_window, sem_t *sem_tire, sem_t *sem_battery, sem_t *sem_body) {
	int num_win, num_tir, num_bat, num_bod;
	num_win = 0; num_tir = 0; num_bat = 0;  num_bod = 0;
	while(num_win < 7 || num_tir < 4 || num_bat < 1 || num_bod < 1) {
		if(num_bod < 1 && getItem(sem_space, space_limit, sem_body)   == 0) num_bod++;
		if(num_win < 7 && getItem(sem_space, space_limit, sem_window) == 0) num_win++;
		if(num_tir < 4 && getItem(sem_space, space_limit, sem_tire)   == 0) num_tir++;
		if(num_bat < 1 && getItem(sem_space, space_limit, sem_battery)== 0) num_bat++;
	}
	sleep(TIME_CAR);
	sem_post(sem_car);
}

