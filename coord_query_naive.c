#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include "record.h"
#include "coord_query.h"

// coord_query_naive.c
// Purpose: Query records by coordinate using naive linear search through the array of records.
// Usage: ./coord_query_naive <dataset-file>

// Structure holding the array of records and its size
struct naive_data
{
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

// Looks up the record closest to the given coordinates using naive linear search
const struct record *lookup_naive(struct naive_data *data, double lon, double lat)
{
  double min_dist = -1;                // minimum squared distance found so far
  const struct record *closest = NULL; // pointer to closest record found so far
  for (int i = 0; i < data->n; ++i)
  {
    double dx = data->rs[i].lon - lon;
    double dy = data->rs[i].lat - lat;
    double dist = dx * dx + dy * dy; // use squared distance for efficiency
    if (closest == NULL || dist < min_dist)
    {
      min_dist = dist;        // update minimum distance
      closest = &data->rs[i]; // update closest record
    }
  }
  return closest; // return pointer to closest record
}

// Main entry point: runs the generic query loop with naive index functions
int main(int argc, char **argv)
{
  return coord_query_loop(argc, argv,
                          (mk_index_fn)mk_naive,
                          (free_index_fn)free_naive,
                          (lookup_fn)lookup_naive);
}
