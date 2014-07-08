/*
Alexander Schnackenberg
aschnackenberg @albany.edu
*/

void process_query_file(FILE *config, FILE *query);//process query file
int nattr(FILE *config, char relation[ATTRIBUTE_LENGTH]);  //returnsthe number of attributes in relname                    
int tuplen(FILE *config, char relation[ATTRIBUTE_LENGTH]);//returns the length of each tuple of specified r
void infattr(FILE *config, char relation[ATTRIBUTE_LENGTH], char attributeName[ATTRIBUTE_LENGTH]);//print type of attribute and length in bytes
int count(char relation[ATTRIBUTE_LENGTH], int tuple_length); //print the number of tuples of the specified relation in the database
void project(FILE *config, char relation[ATTRIBUTE_LENGTH], char attribute[ATTRIBUTE_LENGTH]);
void selectt(FILE *config, char relation[ATTRIBUTE_LENGTH], char attribute[ATTRIBUTE_LENGTH], char relop[RELOP_LENGTH], char *a);
FILE* get_schema_file(FILE *config, char relation[ATTRIBUTE_LENGTH]); //returns schema file for given relation, assumes relation exists
int relation_exists(FILE *config, char relation[ATTRIBUTE_LENGTH]); //returns 1 if relation exists in config file, 0 otherwise
FILE *get_data_file(char relation[ATTRIBUTE_LENGTH]); //returns data file for given relation, assumes relation exists
//void print_tuple(char tuple[400], int num_attributes, attributeLength[MAX_NUM_ATTRIBUTES]);
