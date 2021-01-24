#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#define HASH_TABLE_DEFAULT_SIZE 157
#define HASH_TABLE_MAX_KEY_LENGTH 50

typedef long item_type;
typedef struct hash_table_s hash_table;

#ifdef __cplusplus
extern "C" {
#endif
/*
Initializes a hash table pointed by the double pointer parameter table_addr, with the given size.
*/
    void hash_table_init(hash_table** table_addr, unsigned int size);
#define hash_table_default_init(a) hash_table_init(a, HASH_TABLE_DEFAULT_SIZE)

/*
Removes all items from hash table.
*/
    void hash_table_clean(hash_table* table);

/*
Closes hash table.
*/
    void hash_table_close(hash_table* table);

/*
Copies all items from a source hash table to a destination table.

true: All items copied successfully.
false: Copy of one or more items failed. Uninitialized tables, memory allocation failure
       or items with same key on both tables with overwrite parameter set to false.
*/
    _Bool hash_table_copy(hash_table* dest, const hash_table* source, _Bool overwrite);

/*
Resizes an already initialized hash table pointed by the double pointer parameter table_addr.
*/
    void hash_table_resize(hash_table** table_addr, unsigned int size);

/*
Initializes a hash table pointed by the double pointer parameter dest_addr, as a clone of the source table.
*/
    void hash_table_clone(hash_table** dest_addr, const hash_table* source);

/*
Searches the item with the specified key parameter inside a hash table.

true: Item was found and its value is returned inside out_value parameter if it isn't null.
false: Item was not found. Uninitialized table or abscence of specified key.
*/
    _Bool hash_table_search(const hash_table* table, const char* key, item_type* out_value);

/*
Inserts an item with the specified key and value parameters in a hash table.

true: Item inserted successfully.
false: Insertion failed. Uninitialized table, memory allocation failure or already existing item key.
*/
    _Bool hash_table_insert(hash_table* table, const char* key, item_type value);

/*
Inserts or updates an item with the specified key and value parameters in a hash table.

true: Item inserted/updated successfully.
false: Insertion failed. Uninitialized table or memory allocation failure.
*/
    _Bool hash_table_upsert(hash_table* table, const char* key, item_type value);

/*
Updates the value of an item (identified by the specified key parameter) in a hash table.

true: Item updated successfully.
false: Update failed. Uninitialized table or missing item having the specified key.
*/
    _Bool hash_table_update(hash_table* table, const char* key, item_type value);

/*
Removes an item identified by the specified key parameter from a hash table.

true: Item deleted successfully.
false: Deletion failed. Uninitialized table or missing item having the specified key.
*/
    _Bool hash_table_delete(hash_table* table, const char* key);

/*
Returns size of a hash_table.
*/
    unsigned int hash_table_size(const hash_table* table);

/*
Returns true if table is empty, false otherwise.
*/
    _Bool hash_table_is_empty(const hash_table* table);

/*
Returns amount of stored items in a hash_table.
*/
    unsigned int hash_table_count(const hash_table* table);

#ifdef HASH_TABLE_DEBUG
/*
Writes the entire table structure into a specified filestream. Use it only for debug purposes.
*/
    void hash_table_to_file(hash_table* table, FILE* fs);
#define hash_table_print(a) hash_table_to_file(a, stdout);
#endif
#ifdef __cplusplus
}
#endif
#endif
