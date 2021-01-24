#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "hash_table.h"

typedef struct hash_table_item_s {
	item_type value;
	char* key;
	struct hash_table_item_s* next;
} hash_table_item;

typedef struct hash_table_s {
	unsigned int size;
	hash_table_item** buckets;
} hash_table;

static inline bool relation(const hash_table_item* item, const char* key) {
	return strncmp(key, item->key, HASH_TABLE_MAX_KEY_LENGTH) == 0;
}

static unsigned int hash(const char* key) {
	unsigned int c, hash = 5381;
	while ((c = *key++))
		hash += (hash << 5) + c;
	return hash;
}

static inline bool alloc_item(hash_table_item** prev, const char* key, unsigned int length, item_type value) {
	hash_table_item* item = malloc(sizeof(hash_table_item));
	if (!item)
		return false;
	length++;
	item->key = malloc(length);
	if (!item->key) {
		free(item);
		return false;
	}
	strcpy_s(item->key, length, key);
	item->value = value;
	item->next = NULL;
	*prev = item;
	return true;
}

static hash_table_item** find(const hash_table* table, const char* key) {
	hash_table_item** prev, * item;
	unsigned int index;
	index = hash(key) % table->size;
	prev = &(table->buckets)[index];
	item = *prev;
	while (item && !relation(item, key)) {
		prev = &(item->next);
		item = *prev;
	}
	return prev;
}

static unsigned int digits(int number) {
	unsigned int n = 1;
	while(number /= 10)
		n++;
	return n;
}

void hash_table_init(hash_table** table_addr, unsigned int size) {
	hash_table* table;
	if (size < 2)
		return;
	table = malloc(sizeof(hash_table));
	if (!table)
		return;
	table->buckets = calloc(size, sizeof(hash_table_item*));
	if (!table->buckets) {
		free(table);
		return;
	}
	table->size = size;
	*table_addr = table;
}

void hash_table_clean(hash_table* table) {
	hash_table_item* item, * tmp;
	for (int i = 0; i < table->size; i++) {
		item = table->buckets[i];
		while (item) {
			tmp = item;
			item = item->next;
			free(tmp->key);
			free(tmp);
		}
	}
}

void hash_table_close(hash_table* table) {
	hash_table_clean(table);
	free(table->buckets);
	free(table);
}

bool hash_table_copy(hash_table* dest, const hash_table* source, bool overwrite) {
	hash_table_item* item;
	bool (*fn)(hash_table*, const char*, item_type) = overwrite ? hash_table_upsert : hash_table_insert;
	for (int i = 0; i < source->size; i++) {
		item = source->buckets[i];
		while (item) {
			if (!fn(dest, item->key, item->value))
				return false;
			item = item->next;
		}
	}
	return true;
}

void hash_table_resize(hash_table** table_addr, unsigned int size) {
	hash_table* old_table = *table_addr, * new_table;
	hash_table_init(&new_table, size);
	if (!new_table)
		return;
	if (!hash_table_copy(new_table, old_table, false)) {
		hash_table_close(new_table);
		return;
	}
	*table_addr = new_table;
	hash_table_close(old_table);
}

void hash_table_clone(hash_table** dest_addr, const hash_table* source) {
	hash_table* table;
	hash_table_init(&table, source->size);
	if (!table)
		return;
	if (!hash_table_copy(table, source, false)) {
		hash_table_close(table);
		return;
	}
	*dest_addr = table;
}

bool hash_table_search(const hash_table* table, const char* key, item_type* out_value) {
	hash_table_item* item, ** tmp;
	tmp = find(table, key);
	if (!(item = *tmp))
		return false;
	if (out_value)
		*out_value = item->value;
	return true;
}

bool hash_table_insert(hash_table* table, const char* key, item_type value) {
	hash_table_item* item, ** prev;
	unsigned int length = strnlen(key, HASH_TABLE_MAX_KEY_LENGTH);
	if (length < 1 || key[length] != '\0')
		return false;
	prev = find(table, key);
	item = *prev;
	if (item)
		return false;
	return alloc_item(prev, key, length, value);
}

bool hash_table_upsert(hash_table* table, const char* key, item_type value) {
	hash_table_item* item, ** prev;
	unsigned int length = strnlen(key, HASH_TABLE_MAX_KEY_LENGTH);
	if (length < 1 || key[length] != '\0')
		return false;
	prev = find(table, key);
	item = *prev;
	if (item) {
		item->value = value;
		return true;
	}
	return alloc_item(prev, key, length, value);
}

bool hash_table_update(hash_table* table, const char* key, item_type value) {
	hash_table_item* item, ** tmp;
	tmp = find(table, key);
	if (!(item = *tmp))
		return false;
	item->value = value;
	return true;
}

bool hash_table_delete(hash_table* table, const char* key) {
	hash_table_item* item, ** prev;
	prev = find(table, key);
	item = *prev;
	if (!item)
		return false;
	*prev = item->next;
	free(item->key);
	free(item);
	return true;
}

unsigned int hash_table_size(const hash_table* table) {
	return table->size;
}

bool hash_table_is_empty(const hash_table* table) {
	for (int i = 0; i < table->size; i++)
		if (table->buckets[i])
			return false;
	return true;
}

unsigned int hash_table_count(const hash_table* table) {
	unsigned int count = 0;
	hash_table_item* item;
	for (int i = 0; i < table->size; i++) {
		item = table->buckets[i];
		while (item) {
			count++;
			item = item->next;
		}
	}
	return count;
}

void hash_table_to_file(hash_table* table, FILE* fs) {
	hash_table_item* item;
	unsigned int max_digits, curr_digits;
	max_digits = digits(table->size);
	for (int i = 0; i < table->size; i++) {
		item = table->buckets[i];
		curr_digits = digits(i);
		fputc('[', fs);
		while (curr_digits++ < max_digits)
			fputc('0', fs);
		fprintf(fs, "%u]", i);
		while (item) {
			fprintf(fs, " -> [%s: %ld]", item->key, item->value);
			item = item->next;
		}
		fputc('\n', fs);
	}
}
