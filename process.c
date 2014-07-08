/*
Alexander Schnackenberg
aschnackenberg @albany.edu
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "constants.h"
#include "struct_def.h"
#include "prototypes.h"


void process_query_file(FILE *config, FILE *query){
	char relation[ATTRIBUTE_LENGTH];
	char attribute[ATTRIBUTE_LENGTH];
	char command[MAX_COMMAND_LENGTH];
	char relop[RELOP_LENGTH];
	char attr[MAX_TUPLE_LENGTH];

	//process query file
	fscanf(query, "%s", command);
	if (strcmp(command, "quit")==0){
		return;
	}
	while(strcmp(command, "quit")!=0){
		//do commands
		if (strcmp(command, "nattr") == 0){
			fscanf(query, "%s", relation);
			//check to see if relation exists in config file
			if(!relation_exists(config, relation)){
				printf("Error: invalid relation -- %s \n\n", relation); process_query_file(config, query);fflush(stdout);
			}			
			printf("%d \n\n", nattr(config, relation));fflush(stdout); 
		} else
		if (strcmp(command, "tuplen") == 0){
			fscanf(query, "%s", relation);
			//check to see if relation exists in config file
			if(!relation_exists(config, relation)){
				printf("Error: invalid relation -- %s \n\n", relation); process_query_file(config, query);fflush(stdout);
			}		
			printf("%d \n\n", tuplen(config, relation));fflush(stdout);
		} else
		if (strcmp(command, "infattr") == 0){
			fscanf(query, "%s %s", relation, attribute);
			//check to see if relation exists in config file
			if(!relation_exists(config, relation)){
				printf("Error: invalid relation -- %s \n\n", relation); process_query_file(config, query);fflush(stdout);
			}		
			infattr(config, relation, attribute);
		} else
		if (strcmp(command, "count") == 0){
			fscanf(query, "%s", relation);
			//check to see if relation exists in config file
			if(!relation_exists(config, relation)){
				printf("Error: invalid relation -- %s \n\n", relation); process_query_file(config, query);fflush(stdout);
			}
			printf("%d \n\n", count(relation, tuplen(config, relation)));fflush(stdout);
		} else
		if (strcmp(command, "project") == 0){
			fscanf(query, "%s %s", relation, attribute);
			//check to see if relation exists in config file
			if(!relation_exists(config, relation)){
				printf("Error: invalid relation -- %s \n\n", relation); process_query_file(config, query);fflush(stdout);
			}
			project(config, relation, attribute);
		} else
		if (strcmp(command, "select") == 0){
			fscanf(query, "%s %s %s %s", relation, attribute, relop, attr);
			//check to see if relation exists in config file
			if(!relation_exists(config, relation)){
				printf("Error: invalid relation -- %s \n\n", relation); process_query_file(config, query);fflush(stdout);
			}
			selectt(config, relation, attribute, relop, attr);
		}
		fscanf(query, "%s", command);
		if (strcmp(command, "quit")==0){
			exit(1);
		}	
	}
}

/*
prints out number of attributes for specified relation
assumes that relation exists in config file
*/
int nattr(FILE *config, char relation[ATTRIBUTE_LENGTH]){
	int num_attributes = 0;
	FILE *schema = get_schema_file(config, relation);
	fscanf(schema, "%d", &num_attributes);
	return num_attributes;
}

/*
prints the length of each tuple of specified relation
assumes relation exists in config file
*/
int tuplen(FILE *config, char relation[ATTRIBUTE_LENGTH]){
	int num_attributes = 0;
	int total_length = 0;
	char type;
	int attrlen = 0;
	int i = 0;
	char attribute[ATTRIBUTE_LENGTH];
	FILE *schema = get_schema_file(config, relation);
	
	fscanf(schema, "%d", &num_attributes);
	while(i<num_attributes){
		fscanf(schema,"%s %c %d", attribute, &type, &attrlen);
		total_length += attrlen;
		i++;
	}
	return total_length;
}

/*
print the type of the attribute and its length in bytes
assumes relation exists in config file
*/
void infattr(FILE *config, char relation[ATTRIBUTE_LENGTH], char attribute[ATTRIBUTE_LENGTH]){
	int num_attributes = 0;
	char type;
	int i = 0;
	int attrlen = 0;
	char attr[ATTRIBUTE_LENGTH];
	FILE *schema = get_schema_file(config, relation);
	fscanf(schema, "%d", &num_attributes); //read in that first line, do nothing with num_attributes
	while(i<(num_attributes)){
		fscanf(schema,"%s %c %d", attr, &type, &attrlen);		
		if(strcmp(attr, attribute)==0){
			printf("Attribute type  : %c\nAttribute length: %d\n\n", type, attrlen);fflush(stdout);
			return;
		}
		i++;
	}
	//since we haven't returned, we know the attribute is not in the relation
	printf("Error: Invalid attribute -- %s\n\n", attribute);fflush(stdout);
}

/*
print the number of tuples of the specified relation in the database
*/
int count(char relation[ATTRIBUTE_LENGTH], int tuple_length){
	int count = 0;
	char c[MAX_TUPLE_LENGTH];
	FILE *data = get_data_file(relation);
	while(fread(c,sizeof(char), tuple_length, data)){				
		count++;
	}
	return count;
}

/*
compute the projection of the specified relation over the specified attribute and print the projected values
eliminate duplicate values before printing the output to stdout
assumes relation exists
>>figure out what the offset is. loop through schema, add up values until attribute. 
>>get count. from i = 0 to count, attribute is located at offset + i*tuplelength 
*/
void project(FILE *config, char relation[ATTRIBUTE_LENGTH], char attribute[ATTRIBUTE_LENGTH]){	
	FILE *schema = get_schema_file(config, relation);	
	FILE *data =get_data_file(relation);
	node *head;
	node *temp;
  node *curr;
	int intAttribute = 0;
	char stringAttribute[MAX_TUPLE_LENGTH];
	char attr[ATTRIBUTE_LENGTH];
	int i;
	int j = 0;
	char type;
	char t;
	int offset = 0;
	int attrlen = 0;
	int alen = 0;
	int num_attributes = 0;
	int tuple_length = 0;
	int single_node = 0;
	head = NULL;
	temp = NULL;
	curr = NULL;
	

	
	fscanf(schema, "%d", &num_attributes);
	while(j<num_attributes){
		fscanf(schema,"%s %c %d", attr, &type, &attrlen);		
		if (strcmp(attr, attribute)==0){			
			offset = tuple_length;
			t = type;
			alen = attrlen;		
		}
		tuple_length += attrlen;
		j++;
	}
	if(alen==0){
		printf("Error: Invalid attribute -- %s\n\n", attribute);fflush(stdout);
		return;
	}
	//process data file
	for(i = 0; i<count(relation, tuple_length); i++){
		curr = head;		
		if ((temp = (node *)malloc(sizeof(node))) == NULL){
			printf("Node allocation failed. \n"); exit(1); //stop program
		}		
		fseek(data, offset, SEEK_SET);		
		if(t == 'S'){
			fread(stringAttribute, sizeof(char), alen, data);
			//list has no length			
			if (head == NULL){
				head = temp;
				strcpy(head->string_attribute, stringAttribute);
				single_node = 1;		
			}
			else{
				//list has at least on node, loop through to see if string occurs
				while(strcmp(stringAttribute, curr->string_attribute)!=0){
					if(curr->next!=NULL){					
						curr=curr->next;
					}
					else{
						strcpy(temp->string_attribute, stringAttribute);
						curr->next = temp;
						if(single_node==1){
							head->next = temp;
							single_node = 0;	
						}
						break;
					}					
				}																	
			}	
		}
		else{
			fread(&intAttribute, sizeof(int), 1, data);
			//list has no length
			if (head == NULL){
				head = temp;
				head->int_attribute = intAttribute;
				single_node = 1;
			}
			else{							
				while(intAttribute != curr->int_attribute){
					if (curr->next != NULL){
						curr = curr->next;
					}
					else{
						temp->int_attribute = intAttribute;
						curr->next = temp;
						if(single_node==1){
							head->next = temp;
							single_node = 0;	
						}
						break;
					}
				}
			}		
		}
		offset += tuple_length;
		//free(temp);
	}
	curr = head;
	
	while(curr!=NULL){
		if (t == 'S'){
			printf("%s \n", curr->string_attribute);fflush(stdout);
		}
		else{
			printf("%d \n",curr->int_attribute);fflush(stdout);
		}
		curr = curr->next;
	}
	printf("\n");
}

/*

*/
void selectt(FILE *config, char relation[ATTRIBUTE_LENGTH], char attribute[ATTRIBUTE_LENGTH], char relop[RELOP_LENGTH], char a[MAX_TUPLE_LENGTH]){
	FILE *schema = get_schema_file(config, relation);	
	FILE *data =get_data_file(relation);
	int intAttribute = 0;
	char stringAttribute[MAX_TUPLE_LENGTH];
	char attr[ATTRIBUTE_LENGTH];
	int i;
	char type;
	char t;
	int offset = 0;
	int off = 0;
	int attrlen = 0;
	int alen = 0;
	int num_attributes = 0;
	int tuple_length = 0;
	char string[MAX_TUPLE_LENGTH];
	int integer = 0;
	int j = 0;
	char *token;
	int attributeLength[MAX_NUM_ATTRIBUTES];
	char attributeType[MAX_NUM_ATTRIBUTES];
	int k = 0;
	int no_tuples = 1;
	
	fscanf(schema, "%d", &num_attributes);
	while(j<num_attributes){
		fscanf(schema,"%s %c %d", attr, &type, &attrlen);		
		attributeLength[j] = attrlen;	
		attributeType[j] = type;			
		if (strcmp(attr, attribute)==0){			
			offset = tuple_length;
			t = type;
			alen = attrlen;		
		}
		tuple_length += attrlen;
		j++;
	}
	if(alen==0){
		printf("Error: Invalid attribute -- %s\n\n", attribute);fflush(stdout);
		return;
	}
	if(t=='S' && ((strcmp(relop, "!=")!=0)&&(strcmp(relop, "==")!=0))){
		printf("Error: Wrong type of value for attribute %s\n\n", attribute);fflush(stdout);
		return;
	}
	if(t=='I' && a[0] == '\"'){
		printf("Error: Wrong type of value for attribute %s\n\n", attribute);fflush(stdout);
		return;
	}
	off = offset;
	for(i = 0; i<count(relation, tuple_length); i++){	
		fseek(data, off, SEEK_SET);				
		if(t == 'S'){
			token = strtok(a, "\"");
			fread(stringAttribute, sizeof(char), alen, data);
			if(strcmp(relop, "!=")==0){				
				if(strcmp(stringAttribute, token)!=0){
					no_tuples = 0;				
					fseek(data, off-offset, SEEK_SET);
					k=0;					
					while(k<num_attributes){
						if(attributeType[k]=='S'){						
							fread(string, sizeof(char), attributeLength[k], data);
							printf("%s ", string);
						}
						else{
							fread(&integer, sizeof(int), 1, data);
							printf("%d ", integer);
						}						
						k++;
					}
					printf("\n");			
				}
			}
			else{
				if(strcmp(relop, "==")==0){
					if(strcmp(stringAttribute, token)==0){
						no_tuples = 0;				
						fseek(data, off-offset, SEEK_SET);
						k=0;					
						while(k<num_attributes){
							if(attributeType[k]=='S'){						
								fread(string, sizeof(char), attributeLength[k], data);
								printf("%s ", string);
							}
							else{
								fread(&integer, sizeof(int), 1, data);
								printf("%d ", integer);
							}						
							k++;
						}
						printf("\n");	
					}
				
			}
		}}
		else{
			fread(&intAttribute, sizeof(int), 1, data);			
			if((strcmp(relop, "==")==0) && (intAttribute == atoi(a))){ // relop: ==
				no_tuples = 0;				
				fseek(data, off-offset, SEEK_SET);
				k=0;					
				while(k<num_attributes){
					if(attributeType[k]=='S'){						
						fread(string, sizeof(char), attributeLength[k], data);
						printf("%s ", string);
					}
					else{
						fread(&integer, sizeof(int), 1, data);
						printf("%d ", integer);
					}						
					k++;
				}
				printf("\n");
			} else
			if((strcmp(relop, "!=")==0) && (intAttribute != atoi(a))){ //relop: !=
				no_tuples = 0;				
				fseek(data, off-offset, SEEK_SET);
				k=0;					
				while(k<num_attributes){
					if(attributeType[k]=='S'){						
						fread(string, sizeof(char), attributeLength[k], data);
						printf("%s ", string);
					}
					else{
						fread(&integer, sizeof(int), 1, data);
						printf("%d ", integer);
					}						
					k++;
				}
				printf("\n");
			} else
			if((strcmp(relop, "<=")==0) && (intAttribute <= atoi(a))){ //relop: <=
				no_tuples = 0;				
				fseek(data, off-offset, SEEK_SET);
				k=0;					
				while(k<num_attributes){
					if(attributeType[k]=='S'){						
						fread(string, sizeof(char), attributeLength[k], data);
						printf("%s ", string);
					}
					else{
						fread(&integer, sizeof(int), 1, data);
						printf("%d ", integer);
					}						
					k++;
				}
				printf("\n");
			}	else
			if((strcmp(relop, ">=")==0) && (intAttribute >= atoi(a))){ //relop: >=
				no_tuples = 0;				
				fseek(data, off-offset, SEEK_SET);
				k=0;					
				while(k<num_attributes){
					if(attributeType[k]=='S'){						
						fread(string, sizeof(char), attributeLength[k], data);
						printf("%s ", string);
					}
					else{
						fread(&integer, sizeof(int), 1, data);
						printf("%d ", integer);
					}						
					k++;
				}
				printf("\n");
			}	else
			if((strcmp(relop, ">")==0) && (intAttribute>atoi(a))){ //relop: >
				no_tuples = 0;				
				fseek(data, off-offset, SEEK_SET);
				k=0;					
				while(k<num_attributes){
					if(attributeType[k]=='S'){						
						fread(string, sizeof(char), attributeLength[k], data);
						printf("%s ", string);
					}
					else{
						fread(&integer, sizeof(int), 1, data);
						printf("%d ", integer);
					}						
					k++;
				}
				printf("\n");
			}	else
			if((strcmp(relop, "<")==0) && (intAttribute<atoi(a))){ //relop: <		
				no_tuples = 0;				
				fseek(data, off-offset, SEEK_SET);
				k=0;					
				while(k<num_attributes){
					if(attributeType[k]=='S'){						
						fread(string, sizeof(char), attributeLength[k], data);
						printf("%s ", string);
					}
					else{
						fread(&integer, sizeof(int), 1, data);
						printf("%d ", integer);
					}						
					k++;
				}
				printf("\n");
			}							
		}
		off += tuple_length;	
	}
	if(no_tuples == 1){
		printf("No tuples satisfy the specified condition.\n");
	}
	printf("\n");
}

/*
returns a pointer to the appropriate schema file
assumes that relation exists in config
*/
FILE* get_schema_file(FILE *config, char relation[ATTRIBUTE_LENGTH]){
	FILE* schema; //pointer to file to be returned by function	
	char schema_name[ATTRIBUTE_LENGTH+FILETYPE]; //creates a string of length attribute + buffer to store file name
	
	strcpy(schema_name, relation);
	strcat(schema_name, ".sch");
	if ((schema = fopen(schema_name, "r"))== NULL){
		fprintf(stderr, "Could not open appropriate schema file \n"); exit(1);
	}
	else{
		rewind(schema);
		return schema;
	}
}

/*
checks to see if specified relation exists in config file. return 1 if true, 0 otherwise
*/
int relation_exists(FILE *config, char relation[ATTRIBUTE_LENGTH]){	
	int num_relations = 0;
	char rel[ATTRIBUTE_LENGTH];	
	int i; //used as iterator
	rewind(config);	
	fscanf(config, "%d", &num_relations);
	for(i=0; i<num_relations; i++){
		fscanf(config, "%s", rel);
		if (strcmp(relation, rel)==0){
			return 1;
		}
	}
	return 0; 
}

/*
gets data file for relation, assumes relation exists
*/
FILE *get_data_file(char relation[ATTRIBUTE_LENGTH]){
	FILE *data;	
	char dataFileName[ATTRIBUTE_LENGTH + FILETYPE];
	strcpy(dataFileName, relation);
	strcat(dataFileName, ".dat");
	if ((data = fopen(dataFileName, "rb")) == NULL){
		fprintf(stderr, "Data file could not be opened \n"); exit(1);
	}
	rewind(data);
	return data;
}


