#include "definitions.h"
#include "worker.h"
int initSem();
int destroySem();
void createWorker(int* parts_cnt, pthread_t *workers, work_pack *wpacks, resource_pack *rpack, int jobi);
void initResourcePack(struct resource_pack *pack,
		int space_limit, int num_workers);
void reportResults();

