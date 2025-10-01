#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include "record.h"
#include "id_query.h"

// id_query_naive.c
// Purpose: Query records by ID using naive linear search through the array of records.
// Usage: ./id_query_naive <dataset-file>

// Structure holding the array of records and its size
struct naive_data {
    struct record *rs; // pointer to array of records
    int n;             // number of records
};

// Creates and returns a naive_data struct wrapping the array of records
struct naive_data *mk_naive(struct record *rs, int n)
{
    struct naive_data *data = malloc(sizeof(struct naive_data)); // allocate struct
    if (!data)
        return NULL; // allocation failed
    data->rs = rs; // store pointer to records
    data->n = n;   // store number of records
    return data;
}

// Frees the naive_data struct (does not free the records themselves)
void free_naive(struct naive_data *data)
{
    free(data); // free the wrapper struct
}

// Looks up a record by ID using naive linear search
const struct record *lookup_naive(struct naive_data *data, int64_t needle)
{
    for (int i = 0; i < data->n; ++i) {
        if (data->rs[i].osm_id == needle) {
            return &data->rs[i]; // found, return pointer
        }
    }
    return NULL; // not found
}

// Main entry point: runs the generic query loop with naive index functions
int main(int argc, char **argv)
{
    return id_query_loop(argc, argv,
                         (mk_index_fn)mk_naive,
                         (free_index_fn)free_naive,
                         (lookup_fn)lookup_naive);
}
