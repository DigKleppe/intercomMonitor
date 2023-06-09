/*
 * intercomMonitor.h
 *
 *  Created on: Jun 9, 2023
 *      Author: dig
 */

#ifndef INCLUDES_INTERCOMMONITOR_H_
#define INCLUDES_INTERCOMMONITOR_H_

struct thread_info {    /* Used as argument to thread_start() */
        pthread_t thread_id;        /* ID returned by pthread_create() */
        int       thread_num;       /* Application-defined thread # */
        int udpPort;
};

#include "ports.h"

#endif /* INCLUDES_INTERCOMMONITOR_H_ */
