#ifndef REACT_H
#define REACT_H

#define MAXCALLBACKS 3

#include <stdlib.h>
#include <stdbool.h>

typedef int (*compute1) (int);
typedef int (*compute2) (int, int);

typedef void (*callback) (void *, int);
typedef uint8_t callback_id;

typedef struct
{
    callback cb;
    void *info;
} callback_t;

typedef struct cell_t
{
    struct
    {
        compute1 func;
        struct cell_t *a;
    }comp1;

    struct
    {
        compute2 func;
        struct cell_t *a;
        struct cell_t *b;
    }comp2;

    int value;
    bool isCompute;
    struct cell_t *next;
    struct reactor_t *reactor;
    callback_t callbacks[5];

} cell_t;

typedef struct reactor_t
{
    cell_t *head;
} reactor_t;

reactor_t *create_reactor();
void destroy_reactor(reactor_t *);

cell_t *create_input_cell(reactor_t *, int initial_value);
cell_t *create_compute1_cell(reactor_t *, cell_t *, compute1);
cell_t *create_compute2_cell(reactor_t *, cell_t *,
                                  cell_t *, compute2);

int get_cell_value(cell_t *);
void set_cell_value(cell_t *, int new_value);

callback_id add_callback(cell_t *, void *, callback);
void remove_callback(cell_t *, callback_id);

#endif
