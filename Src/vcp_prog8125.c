/**
  ******************************************************************************
  * File Name          : vcp_prog8125.c
  * Description        : Virtual Com Port implementation routines including circular queue
  ******************************************************************************
  *  Created on: Aug 16, 2016
  *  Revised on: Nov 2, 2016
  *      Author: A Smith
  */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <vcp_prog8125.h>


void CDC_Transmit_FS( uint8_t *ptr, size_t len);

extern circularQueue_t  VcpQueue;		// externally defined circular queue definition

// FUNCTION      : _write
// DESCRIPTION   :
//   This function is typically part of newlib and we are overriding it with our own to force all
//   output to stdout to go to the virtual COM port on the stm board.
// PARAMETERS    :
//   fd          : file descriptor to write to (ignored as we override all fds)
//   ptr         : pointer to character buffer to be outputed
//   len         : number of characters at ptr to be sent
// RETURNS       :
//   number of characters actually sent (in this case always assumes successful transmission of len)

int _write(int fd, char *ptr, size_t len){
	CDC_Transmit_FS( (uint8_t *)ptr, len);
	return len;
}

// FUNCTION      : _write_r
// DESCRIPTION   :
//   This function is typically part of newlib and we are overriding it with our own to force all
//   output to stdout to go to the virtual COM port on the stm board.
// PARAMETERS    :
//   reent       : pointer used to manage reentrant code
//   fd          : file descriptor to write to (ignored as we override all fds)
//   ptr         : pointer to character buffer to be outputed
//   len         : number of characters at ptr to be sent
// RETURNS       :
//   number of characters actually sent (in this case always assumes successful transmission of len)

int _write_r(void *reent, int fd, char *ptr, size_t len)
{
	return _write(fd, ptr, len);
}

// FUNCTION      : _read
// DESCRIPTION   :
//   This function is typically part of newlib and we are overriding it with our own to force all
//   input to stdin to come from the virtual COM port on the stm board.
// PARAMETERS    :
//   fd          : file descriptor to read from to (ignored as we override all fds)
//   ptr         : pointer of where data will be put that is read
//   len         : maximum number of characters to read
// RETURNS       :
//   number of characters actually read
int _read( int fd, char *ptr, int len )
{
  uint32_t count;
  uint32_t countMoved = 0;
  if( isVcpEmpty(&VcpQueue))
	  count = 0;
  else
  {
	  count = len;		// assume that len will be filled to start off
	  while( len-- )    // move data from vcp queue to stdin
	  {
		  // stop if no more data in vcp queue
		  if( !isVcpEmpty(&VcpQueue) )
		  {
			  getVcpItem(&VcpQueue, (uint8_t *)ptr);
			  ptr++;
			  countMoved++;
		  }
		  else
		  {
			  // return that only some of len buffer was filled
			  count = countMoved;
		  }
	  }

  }
  return count;
}

// FUNCTION      : _read_r
// DESCRIPTION   :
//   This function is typically part of newlib and we are overriding it with our own to force all
//   input to stdin to come from the virtual COM port on the stm board.
// PARAMETERS    :
//   r           : pointer used to manage reentrant code
//   fd          : file descriptor to read from to (ignored as we override all fds)
//   ptr         : pointer of where data will be put that is read
//   len         : maximum number of characters to read
// RETURNS       :
//   number of characters actually read
int _read_r( void *r, int fd, char *ptr, int len )
{
  return _read( fd, ptr, len);
}



// FUNCTION      : initializeVcpQueue
// DESCRIPTION   :
//   This function will initialize the circular queue data type
// PARAMETERS    :
//   theQueue    : pointer to circular queue to be used
// RETURNS       :
//   Nothing
void initializeVcpQueue(circularQueue_t *theQueue)
{
    int i;
    theQueue->validItems  =  0;
    theQueue->first       =  0;
    theQueue->last        =  0;
    for(i=0; i<MAX_ITEMS; i++)
    {
        theQueue->data[i] = 0;
    }
    return;
}

// FUNCTION      : isVcpEmpty
// DESCRIPTION   :
//   This function will return 0 if queue is not empty, or 1 if it is
// PARAMETERS    :
//   theQueue    : pointer to circular queue to be used
// RETURNS       :
//   return 0 if queue is not empty, or 1 if it is
int isVcpEmpty(circularQueue_t *theQueue)
{
    if(theQueue->validItems==0)
        return(1);
    else
        return(0);
}


// FUNCTION      : checkVcpCount
// DESCRIPTION   :
//   This function will return 0 if queue is not empty, or 1 if it is
// PARAMETERS    :
//   theQueue    : pointer to circular queue to be used
// RETURNS       :
//   number of characters ready to read
int checkVcpCount(circularQueue_t *theQueue)
{
    return(theQueue->validItems);
}

// FUNCTION      : putVcpItem
// DESCRIPTION   :
//   This function will put a byte into the specified queue
// PARAMETERS    :
//   theQueue    : pointer to circular queue to be used
//   theItemValue: byte to be queued
// RETURNS       :
//   return 0 on success
int putVcpItem(circularQueue_t *theQueue, uint8_t theItemValue)
{
    if(theQueue->validItems>=MAX_ITEMS)
    {
 //       printf("The queue is full\n");
 //       printf("You cannot add items\n");
        return(-1);
    }
    else
    {
        theQueue->validItems++;
        theQueue->data[theQueue->last] = theItemValue;
        theQueue->last = (theQueue->last+1)%MAX_ITEMS;
    }
    return 0;
}

// FUNCTION      : putVcpChar
// DESCRIPTION   :
//   This function will put a byte into queue
// PARAMETERS    :
//   theItemValue: byte to be queued
// RETURNS       :
//   return 0 on success
int putVcpChar( uint8_t theItemValue)
{
	return( putVcpItem(&VcpQueue, theItemValue) );

}

// FUNCTION      : getVcpItem
// DESCRIPTION   :
//   This function will get a byte from the specified queue
// PARAMETERS    :
//   theQueue    : pointer to circular queue to be used
//   theItemValue: pointer to byte to be unqueued
// RETURNS       :
//   return 0 on success
int getVcpItem(circularQueue_t *theQueue, uint8_t *theItemValue)
{
    if(isVcpEmpty(theQueue))
    {
//        printf("isempty\n");
        return(-1);
    }
    else
    {
        *theItemValue=theQueue->data[theQueue->first];
        theQueue->first=(theQueue->first+1)%MAX_ITEMS;
        theQueue->validItems--;
    }
    return(0);
}

