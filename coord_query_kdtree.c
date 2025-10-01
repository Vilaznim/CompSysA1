#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include <math.h>

#include "record.h"
#include "coord_query.h"

const int k = 2;

struct kdtree_node {
    struct record *r;
    struct kdtree_node *left;
    struct kdtree_node *right;
};

struct kdtree_data {
    struct kdtree_node *root;
    struct record *records; // Store the copied records
};

int compare_lon(const void* a, const void* b) {
    const struct record* ra = (const struct record*)a;
    const struct record* rb = (const struct record*)b;
    double diff = ra->lon - rb->lon;
    return (diff < 0) ? -1 : (diff > 0) ? 1 : 0;
}

int compare_lat(const void* a, const void* b) {
    const struct record* ra = (const struct record*)a;
    const struct record* rb = (const struct record*)b;
    double diff = ra->lat - rb->lat;
    return (diff < 0) ? -1 : (diff > 0) ? 1 : 0;
}

struct kdtree_node* build_kdtree(struct record* rs, int n, int depth) {
    if (n <= 0) return NULL;

    int axis = depth % k;
    int mid = n / 2;

    if (axis == 0) {
        qsort(rs, n, sizeof(struct record), compare_lon);
    } else {
        qsort(rs, n, sizeof(struct record), compare_lat);
    }

    struct kdtree_node* node = malloc(sizeof(struct kdtree_node));
    node->r = &rs[mid];
    node->left = build_kdtree(rs, mid, depth + 1);
    node->right = build_kdtree(rs + mid + 1, n - mid - 1, depth + 1);

    return node;
}

void free_kdtree(struct kdtree_node* node) {
    if (node == NULL) return;
    free_kdtree(node->left);
    free_kdtree(node->right);
    free(node);
}

const struct record* find_nearest(struct kdtree_node* node, double lon, double lat, 
                                const struct record* best, double* best_dist, int depth) {
    if (node == NULL) return best;

    double dx = node->r->lon - lon;
    double dy = node->r->lat - lat;
    double dist = dx*dx + dy*dy;

    if (best == NULL || dist < *best_dist) {
        *best_dist = dist;
        best = node->r;
    }

    int axis = depth % k;
    double diff = (axis == 0) ? lon - node->r->lon : lat - node->r->lat;
    struct kdtree_node *first = (diff < 0) ? node->left : node->right;
    struct kdtree_node *second = (diff < 0) ? node->right : node->left;

    best = find_nearest(first, lon, lat, best, best_dist, depth + 1);
    
    if (diff * diff < *best_dist) {
        best = find_nearest(second, lon, lat, best, best_dist, depth + 1);
    }

    return best;
}

struct kdtree_data* mk_kdtree(struct record* rs, int n) {
    struct kdtree_data* data = malloc(sizeof(struct kdtree_data));
    if (!data) return NULL;

    // Create a copy of records that we can sort
    data->records = malloc(n * sizeof(struct record));
    if (!data->records) {
        free(data);
        return NULL;
    }
    memcpy(data->records, rs, n * sizeof(struct record));
    
    data->root = build_kdtree(data->records, n, 0);
    return data;
}

void free_kdtree_data(struct kdtree_data* data) {
    if (data == NULL) return;
    free_kdtree(data->root);
    free(data->records);
    free(data);
}

const struct record* lookup_kdtree(struct kdtree_data *data, double lon, double lat) {
    double best_dist = -1;
    return find_nearest(data->root, lon, lat, NULL, &best_dist, 0);
}

int main(int argc, char** argv) {
    return coord_query_loop(argc, argv,
                          (mk_index_fn)mk_kdtree,
                          (free_index_fn)free_kdtree_data,
                          (lookup_fn)lookup_kdtree);
}