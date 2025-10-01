#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>

#include "record.h"
#include "id_query.h"

// id_query_binsort.c
// Purpose: Query records by ID using binary search on a sorted array of records.
// Usage: ./id_query_binsort <dataset-file>

// Structure holding the sorted array of records and its size
struct binsort_data
{
  struct record *rs; // pointer to array of records
  int n;             // number of records
};

// Comparison function for qsort: sorts records by osm_id
static int compare_records(const void *a, const void *b)
{
  const struct record *ra = (const struct record *)a;
  const struct record *rb = (const struct record *)b;
  if (ra->osm_id < rb->osm_id)
    return -1;
  if (ra->osm_id > rb->osm_id)
    return 1;
  return 0;
}

// Creates and returns a binsort_data struct with records sorted by osm_id
struct binsort_data *mk_binsort(struct record *rs, int n)
{
  struct binsort_data *data = malloc(sizeof(struct binsort_data)); // allocate struct
  if (!data)
    return NULL;                                              // allocation failed
  data->rs = rs;                                              // store pointer to records
  data->n = n;                                                // store number of records
  qsort(data->rs, n, sizeof(struct record), compare_records); // sort records by osm_id
  return data;
}

// Frees the binsort_data struct (does not free the records themselves)
void free_binsort(struct binsort_data *data)
{
  free(data); // free the wrapper struct
}

// Looks up a record by ID using binary search
const struct record *lookup_binsort(struct binsort_data *data, int64_t needle)
{
  int left = 0;            // left bound of search
  int right = data->n - 1; // right bound of search
  while (left <= right)
  {
    int mid = left + (right - left) / 2; // middle index
    // Check if mid record matches the needle
    if (data->rs[mid].osm_id == needle)
    {
      return &data->rs[mid]; // found, return pointer
    }
    // If needle is greater, search right half
    if (data->rs[mid].osm_id < needle)
    {
      left = mid + 1;
    }
    else
    {
      right = mid - 1; // else search left half
    }
  }
  return NULL; // not found
}

// Main entry point: runs the generic query loop with binsort index functions
int main(int argc, char **argv)
{
  return id_query_loop(argc, argv,
                       (mk_index_fn)mk_binsort,
                       (free_index_fn)free_binsort,
                       (lookup_fn)lookup_binsort);
}