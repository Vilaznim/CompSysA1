#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include "record.h"
#include "id_query.h"

// id_query_indexed.c
// Purpose: Query records by ID using an array of index_record structs (linear search).
// Usage: ./id_query_indexed <dataset-file>

// Structure representing an index entry for a record
struct index_record
{
    int64_t osm_id;              // OSM ID of the record
    const struct record *record; // Pointer to the actual record
};

// Structure holding the array of index_record and its size
struct indexed_data
{
    struct index_record *irs; // pointer to array of index_record
    int n;                    // number of records
};

// Function declarations
struct indexed_data *mk_indexed(struct record *rs, int n);
void free_indexed(struct indexed_data *data);
const struct record *lookup_indexed(struct indexed_data *data, int64_t needle);

// Comparison function for qsort: sorts index_record by osm_id
static int compare_index_records(const void *a, const void *b)
{
    const struct index_record *ia = (const struct index_record *)a;
    const struct index_record *ib = (const struct index_record *)b;
    if (ia->osm_id < ib->osm_id)
        return -1;
    if (ia->osm_id > ib->osm_id)
        return 1;
    return 0;
}

// Creates and returns an indexed_data struct with index_record array sorted by osm_id
struct indexed_data *mk_indexed(struct record *rs, int n)
{
    struct indexed_data *data = malloc(sizeof(struct indexed_data)); // allocate struct
    if (!data)
        return NULL; // allocation failed

    data->irs = malloc(n * sizeof(struct index_record)); // allocate index_record array
    if (!data->irs)
    {
        free(data);
        return NULL;
    }

    // Initialize index records
    for (int i = 0; i < n; i++)
    {
        data->irs[i].osm_id = rs[i].osm_id;
        data->irs[i].record = &rs[i];
    }

    qsort(data->irs, n, sizeof(struct index_record), compare_index_records); // sort index by osm_id
    data->n = n;
    return data;
}

// Frees the indexed_data struct and its index_record array
void free_indexed(struct indexed_data *data)
{
    if (data)
    {
        free(data->irs); // free index_record array
        free(data);      // free wrapper struct
    }
}

// Looks up a record by ID using linear search on the index_record array
const struct record *lookup_indexed(struct indexed_data *data, int64_t needle)
{
    for (int i = 0; i < data->n; ++i)
    {
        if (data->irs[i].osm_id == needle)
        {
            return data->irs[i].record; // found, return pointer
        }
    }
    return NULL; // not found
}

// Main entry point: runs the generic query loop with indexed index functions
int main(int argc, char **argv)
{
    return id_query_loop(argc, argv,
                         (mk_index_fn)mk_indexed,
                         (free_index_fn)free_indexed,
                         (lookup_fn)lookup_indexed);
}