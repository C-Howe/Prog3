/*
Alexander Schnackenberg
aschnackenberg @albany.edu
*/

#include <stdio.h>
#include <stdlib.h>
#include "constants.h"
#include "prototypes.h"

int main(int argc, char* argv[]){
	FILE *config;
	FILE *query;	
	
	if (argc!= NUM_ARGS){
		fprintf(stderr, "Incorrect number of command line arguments \n"); exit(1);		
	}
	if ((config = fopen(argv[CONFIG], "r"))== NULL){
		fprintf(stderr, "Could not open configuration file \n"); exit(1);
	}
	if((query = fopen(argv[QUERY], "r"))==NULL){
		fprintf(stderr, "Could not open query file \n"); exit(1);
	}
	
	process_query_file(config, query);
	
	return 0;
}
