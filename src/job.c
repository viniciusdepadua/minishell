//
// Created by araujvin on 15/07/22.
//

#include "job.h"

Job *first_job = NULL;

Job* find_job(pid_t pgid){
    Job* j;

    for(j = first_job; j; j = j->next)
        if(j->pgid == pgid)
            return j;

}

int job_is_stopped (Job *j){
    Process *p;

    for (p = j->first_process; p; p = p->next)
        if (!p->completed && !p->stopped)
            return 0;
    return 1;
}

int job_is_completed(Job *j){
    Process *p;

    for(p = j->first_process; p; p = p->next)
        if(!p->completed)
            return 0;
    return 1;
}