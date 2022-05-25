#include "react.h"

callback_id add_callback(cell_t *c, void *info, callback cb)
{
   uint8_t i;
   for(i = 0; i < MAXCALLBACKS; i++)
   {
      if(c->callbacks[i].cb == NULL)
      {
         c->callbacks[i].cb = cb;
         c->callbacks[i].info = info;
         break;
      }
   }
   return i;
}

void remove_callback(cell_t *c, callback_id id)
{
   c->callbacks[id].cb = NULL;
}

void trigger_callbacks(cell_t *c)
{
   for(callback_id id = 0; id < MAXCALLBACKS; id++)
   {
      if(c->callbacks[id].cb != NULL)
      {
         c->callbacks[id].cb(c->callbacks[id].info, c->value);
      }
   }
}

void update_values(cell_t *c)
{
   if(c != NULL)
   {
      update_values(c->next);
      if(c->isCompute)
      {
         int old_value = c->value;

         if(c->comp1.func != NULL)
         {
            c->value = c->comp1.func(c->comp1.a->value);
         }

         else if(c->comp2.func != NULL)
         {
            c->value = c->comp2.func(c->comp2.a->value, c->comp2.b->value);
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

void add_cell(reactor_t *r, cell_t *c)
{
   c->next = r->head;
   r->head = c;
}

cell_t *alloc_cell(reactor_t *r)
{
   cell_t *c = calloc(1, sizeof(cell_t));
   c->reactor = r;
   add_cell(r, c);
   return c;
}

cell_t *create_compute2_cell(reactor_t *r, cell_t *in_a, cell_t *in_b, compute2 func)
{
   cell_t *c = alloc_cell(r);
   c->value = func(in_a->value, in_b->value);
   c->comp2.func = func;
   c->comp2.a = in_a;
   c->comp2.b = in_b;
   c->isCompute = true;
   return c;
}

cell_t *create_compute1_cell(reactor_t *r, cell_t *in, compute1 func)
{
   cell_t *c = alloc_cell(r);
   c->value = func(in->value);
   c->comp1.func = func;
   c->comp1.a = in;
   c->isCompute = true;
   return c;
}

cell_t *create_input_cell(reactor_t *r, int init_val)
{
   cell_t *c = alloc_cell(r);
   c->value = init_val;
   return c;
}

reactor_t *create_reactor()
{
   reactor_t *r = calloc(1, sizeof(reactor_t));
   return r;
}

void destroy_cells(cell_t *c)
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
