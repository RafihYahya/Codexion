#include "main.h"

 int add_task(struct s_scheduler *sch, int id, long deadline)
 {
    
 }
 
int init_fifo_scheduler(struct s_globalstate *gstate)
{
    struct s_fifo_queue *fifo_q;

    gstate->scheduler->data = malloc(sizeof(struct s_fifo_queue));
    if (!gstate->scheduler->data)
    {
        ERROR("Failed Alloc of scheduler queue Fifo");
        return (-1);
    }
    fifo_q = (struct s_fifo_queue *)gstate->scheduler->data;
    fifo_q->back = NULL;
    fifo_q->front = NULL;
    fifo_q->count = 0;
    gstate->scheduler->add_task = add_task ;
    gstate->scheduler->task_finished = NULL; // wrong

    return (0);
}