/*
 * vcp_prog8125.h
 *
 *  Created on: Aug 16, 2016
 *      Author: smith
 */

#ifndef VCP_PROG8125_H_
#define VCP_PROG8125_H_


// define queue type and size
#define MAX_ITEMS    64
typedef struct circularQueue_s
{
    int     first;
    int     last;
    int     validItems;
    uint8_t     data[MAX_ITEMS];
} circularQueue_t;

// prototypes for queue
void initializeVcpQueue(circularQueue_t *theQueue);
int isVcpEmpty(circularQueue_t *theQueue);
int checkVcpCount(circularQueue_t *theQueue);
int putVcpItem(circularQueue_t *theQueue, uint8_t theItemValue);
int getVcpItem(circularQueue_t *theQueue, uint8_t *theItemValue);

#endif /* VCP_PROG8125_H_ */
