#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>

#include "record.h"
#include "id_query.h"

struct binsort_data {
  struct record *rs;
  int n;
};

static int compare_records(const void* a, const void* b) {
  const struct record* ra = (const struct record*)a;
  const struct record* rb = (const struct record*)b;
  if (ra->osm_id < rb->osm_id) return -1;
  if (ra->osm_id > rb->osm_id) return 1;
  return 0;
}

struct binsort_data *mk_binsort(struct record *rs, int n)
{
  struct binsort_data *data = malloc(sizeof(struct binsort_data));
  if (!data)
    return NULL;
  data->rs = rs;
  data->n = n;
  // Sort the records array by osm_id
  qsort(data->rs, n, sizeof(struct record), compare_records);
  return data;
}

void free_binsort(struct binsort_data *data)
{
    free(data);
}

const struct record *lookup_binsort(struct binsort_data *data, int64_t needle)
{
  int left = 0;
  int right = data->n - 1;
  while (left <= right) {
    int mid = left + (right - left) / 2;
    if (data->rs[mid].osm_id == needle) {
      return &data->rs[mid];
    }
    if (data->rs[mid].osm_id < needle) {
      left = mid + 1;
    } else {
      right = mid - 1;
    }
  }
  return NULL;
}

int main(int argc, char **argv)
{
    return id_query_loop(argc, argv,
                         (mk_index_fn)mk_binsort,
                         (free_index_fn)free_binsort,
                         (lookup_fn)lookup_binsort);
}