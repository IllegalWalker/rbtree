#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>


#include "rbtree.h"

#define KEY_RANGE 100

typedef struct _Data{
	int key;
}Data;

typedef enum {
	TT_AUTO = 0,
	TT_MANUAL = 1,
	TT_ALL = 2,
}TestType;

typedef enum {
	OT_ADD = 0,
	OT_DEL = 1,
	OT_SEARCH = 2,
	OT_ALL = 3,
}OperationType;

int cmp(const Data *data1, const Data *data2)
{
	return data1->key - data2->key;
}

int main(int argc, char *args[]) {
	int test_type = 0;
	puts("Please choose testing type you want: 0 or 1");
	puts("0. automatically.");
	puts("1. manually.");

	printf("select:");
	scanf("%d", &test_type);

	puts("Testing begin: ------------- ");

	Data * data = NULL;
	struct rb_root *root = rb_create_tree((RB_COMPARE_FUNC)cmp);

	if(test_type == TT_AUTO)
	{
		int key;
		int i, count = 100;

		// Indicate whether key is exist.
		int keys[KEY_RANGE];
		memset(keys, 0, sizeof(int)*KEY_RANGE);	// Initialize keys.

		srand(time(NULL));

		for (i = 0; i < count; ++i)
		{
			OperationType ops_type = rand() % OT_ALL;

			key = rand() % KEY_RANGE;
			bool exist = keys[key];

			data = malloc(sizeof(Data));
			data->key = key;

			if (ops_type == OT_ADD)
			{
				Data *new_data = malloc(sizeof(Data));
				new_data->key = key;
				bool res = rb_insert_data(new_data, root);
				if (exist)
				{
					assert(res == false);
					printf("[%d] insert repeated key %d failed!\n", i, key);
				}
				else
				{
					assert(res == true);
					keys[key] = true;	// set key as used.
					printf("[%d] insert key %d success!\n", i, key);
				}
			}
			else if (ops_type == OT_DEL)
			{
				bool res = rb_delete_data(data, root);
				if (exist)
				{
					assert(res == true);
					keys[key] = false;
					printf("[%d] delete existed key %d success!\n", i, key);
				}
				else
				{
					assert(res == false);
					printf("[%d] delete non-existed key %d failed!\n", i, key);
				}
			}
			else if (ops_type == OT_SEARCH)
			{
				struct rb_node * res = rb_search(root, data);
				if (exist)
				{
					assert(res != NULL && ((Data *)(res->data))->key == key);
					printf("[%d] search existed key %d success!\n", i, key);
				}
				else
				{
					assert(res == NULL);
					printf("[%d] search non-existed key %d failed!\n", i, key);
				}
			}

			free(data);
			data = NULL;
		}
	}
	else if(test_type == TT_MANUAL)
	{
		int ops_type = 0;
		int key = 0;
		while(true)
		{
			puts("\nselect operation you want: ");
			puts("0: add.");
			puts("1: delete.");
			puts("2: search.");
			printf("select :");
			scanf("%d", &ops_type);

			printf("Input key:");
			scanf("%d", &key);

			data = malloc(sizeof(Data));
			data->key = key;

			if (ops_type == OT_ADD)
			{
				Data *new_data = malloc(sizeof(Data));
				new_data->key = key;
				bool res = rb_insert_data(new_data, root);
				printf("insert key %d, result: %d!\n", key, res);
			}
			else if (ops_type == OT_DEL)
			{
				bool res = rb_delete_data(data, root);
				printf("delete key %d, result: %d!\n", key, res);
			}
			else if (ops_type == OT_SEARCH)
			{
				struct rb_node * res = rb_search(root, data);

				if(res && res->data && ((Data *)(res->data))->key == key)
				{
					printf("search key %d succeed!\n", key);
				}
				else
				{
					printf("search key %d failed!\n", key);
				}
			}
			else
			{
				puts("input error!");
				break;
			}

			free(data);
		}
	}
	else
	{
		puts("input error.");
		exit(EXIT_SUCCESS);
	}

	rb_free_tree(root);
	root = NULL;
	puts("Testing end: ------------- succeed.");

	return EXIT_SUCCESS;
}
