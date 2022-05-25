#include "react.h"

callback_id add_callback(cell_t *c, void *info, callback cb)
{
   callback_id id;
   for(id = 0; id < MAXCALLBACKS; id++)
   {
      if(c->callbacks[id].cb == NULL)
      {
         c->callbacks[id].cb = cb;
         c->callbacks[id].info = info;
         break;
      }
   }
   return id;
}

void remove_callback(cell_t *c, callback_id id)
{
   c->callbacks[id].cb = NULL;
}

static void trigger_callbacks(cell_t *c)
{
   for(callback_id id = 0; id < MAXCALLBACKS; id++)
   {
      if(c->callbacks[id].cb != NULL)
      {
         c->callbacks[id].cb(c->callbacks[id].info, c->value);
      }
   }
}

static void update_values(cell_t *c)
{
   if(c != NULL)
   {
      update_values(c->next);
      if(c->isCompute)
      {
         int old_value = c->value;

         if(c->compute1.function != NULL)
         {
            c->value = c->compute1.function(c->compute1.input->value);
         }

         else if(c->compute2.function != NULL)
         {
            c->value = c->compute2.function(c->compute2.inputA->value, c->compute2.inputB->value);
         }

         if(c->value != old_value)
         {
            trigger_callbacks(c);
         }
      }
   }
}

void set_cell_value(cell_t *c, int value)
{
   c->value = value;
   trigger_callbacks(c);
   update_values(c->reactor->head);
}

int get_cell_value(cell_t *cell)
{
   return cell->value;
}

static void add_cell(reactor_t *r, cell_t *c)
{
   c->next = r->head;
   r->head = c;
}

static cell_t *allocate_cell(reactor_t *r)
{
   cell_t *c = calloc(1, sizeof(cell_t));
   c->reactor = r;
   add_cell(r, c);
   return c;
}

cell_t *create_compute2_cell(reactor_t *r, cell_t *inputA, cell_t *inputB, compute2 function)
{
   cell_t *c = allocate_cell(r);
   c->value = function(inputA->value, inputB->value);
   c->compute2.function = function;
   c->compute2.inputA = inputA;
   c->compute2.inputB = inputB;
   c->isCompute = true;
   return c;
}

cell_t *create_compute1_cell(reactor_t *r, cell_t *input, compute1 function)
{
   cell_t *c = allocate_cell(r);
   c->value = function(input->value);
   c->compute1.function = function;
   c->compute1.input = input;
   c->isCompute = true;
   return c;
}

cell_t *create_input_cell(reactor_t *r, int initialValue)
{
   cell_t *c = allocate_cell(r);
   c->value = initialValue;
   return c;
}

reactor_t *create_reactor()
{
   reactor_t *r = calloc(1, sizeof(reactor_t));
   return r;
}

static void destroy_cells(cell_t *c)
{
   if(c != NULL)
   {
      destroy_cells(c->next);
      free(c);
   }
}

void destroy_reactor(reactor_t *r)
{
   if(r->head != NULL)
   {
      destroy_cells(r->head);
   }
   free(r);
}
