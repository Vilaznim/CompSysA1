#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>

#include "record.h"
#include "coord_query.h"

struct naive_data {
  struct record *rs;
  int n;
};

struct naive_data* mk_naive(struct record* rs, int n) 
{
  struct naive_data *data = malloc(sizeof(struct naive_data));
  if (!data)
    return NULL;
  data->rs = rs;
  data->n = n;
  return data;
}

void free_naive(struct naive_data* data) {
{
  free(data);
}
}

const struct record* lookup_naive(struct naive_data *data, double lon, double lat) {
  double min_dist = -1;
  const struct record *closest = NULL;
  for (int i = 0; i < data->n; ++i) {
    double dx = data->rs[i].lon - lon;
    double dy = data->rs[i].lat - lat;
    double dist = dx*dx + dy*dy;
    if (closest == NULL || dist < min_dist) {
      closest = &data->rs[i];
      min_dist = dist;
    }
  }
  return closest;
}

int main(int argc, char** argv) {
  return coord_query_loop(argc, argv,
                          (mk_index_fn)mk_naive,
                          (free_index_fn)free_naive,
                          (lookup_fn)lookup_naive);
}
