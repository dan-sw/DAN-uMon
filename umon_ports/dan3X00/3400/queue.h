/***************************************************************************
Copyright ©  2008 Design Art, INC.

The information contained in this work is proprietary and confidential
information of Design Art, Inc. Any unauthorized reproduction, use or
disclosure of this material, or any part thereof, is strictly prohibited.
This work and information is intended solely for the internal use of
authorized Design Art customers, for the limited purposes for which this
work has been provided for.
***************************************************************************/


/*@@I_queue
<TITLE Common Definitions>
    Generic Queue and List functions for double and single lists

*/


#ifndef _I_QUEUE_H
#define _I_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif


// A node in the double linked list
typedef struct _Node
{
	struct _Node *pNext;		//Pointer to next element in the list
	struct _Node *pPrevious;	//Pointer to previous element in the list
} NODE_t, *PNODE_t;

// A node in the single linked list
typedef struct _SNode
{
	struct _SNode *pNext;		//Pointer to next element in the list
} SNODE_t, *PSNODE_t;

// A queue element composed of double linked elements
typedef struct _Queue
{
	NODE_t  *pHead;				//Pointer to head element in the queue
	NODE_t  *pTail;				//Pointer to tail element in the queue
	UINT16  NumOfElements;		//Number of elements in the queue
	UINT16  pad;
} QUEUE_t, *PQUEUE_t;

// A queue element composed of single linked elements
typedef struct _SQueue
{
	SNODE_t  *pHead;			//Pointer to head element in the queue
	SNODE_t  *pTail;			//Pointer to tail element in the queue
	UINT16  NumOfElements;		//Number of elements in the queue
	UINT16  pad;
} SQUEUE_t, *PSQUEUE_t;




static inline VOID QUEUE_init(QUEUE_t * pQueue);
static inline VOID SQUEUE_init(SQUEUE_t * pQueue);
static inline NODE_t *QUEUE_dequeue(QUEUE_t * pQueue);
static inline SNODE_t *SQUEUE_dequeue(SQUEUE_t * pQueue);
static inline VOID QUEUE_release_element(QUEUE_t * pQueue, NODE_t * pData);
static inline VOID SQUEUE_release_element(SQUEUE_t * pQueue, SNODE_t * pData);
static inline VOID QUEUE_enqueue(QUEUE_t * pQueue, NODE_t * pData);
static inline VOID SQUEUE_enqueue(SQUEUE_t * pQueue, SNODE_t * pData);
static inline VOID QUEUE_attach(QUEUE_t * pQueue, NODE_t * pNode, NODE_t * pData);
static inline VOID SQUEUE_attach(SQUEUE_t * pQueue, SNODE_t * pNode, SNODE_t * pData);
static inline VOID QUEUE_enqueue_before(QUEUE_t * pQueue, NODE_t * pNode, NODE_t * pData);
static inline NODE_t *QUEUE_get_first(QUEUE_t * pQueue);
static inline SNODE_t *SQUEUE_get_first(SQUEUE_t * pQueue);
static inline NODE_t *QUEUE_get_last(QUEUE_t * pQueue);
static inline SNODE_t *SQUEUE_get_last(SQUEUE_t * pQueue);
static inline NODE_t *QUEUE_get_next(QUEUE_t * pQueue, NODE_t * pData);
static inline SNODE_t *SQUEUE_get_next(SQUEUE_t * pQueue, SNODE_t * pData);
static inline NODE_t *QUEUE_get_previous(QUEUE_t * pQueue, NODE_t * pData);
static inline QUEUE_t *QUEUE_concat(QUEUE_t * pQueue1, QUEUE_t * pQueue2);
static inline SQUEUE_t *SQUEUE_concat(SQUEUE_t * pQueue1, SQUEUE_t * pQueue2);
static inline NODE_t *QUEUE_detach(QUEUE_t * pQueue, UINT16 NumOfElements);
static inline SNODE_t *SQUEUE_detach(SQUEUE_t * pQueue, UINT16 NumOfElements);
static inline BOOL QUEUE_isHead(QUEUE_t * pQueue, NODE_t * pData);


/*
 *---------------------------------------------------------------------------
 *                     PROTOTYPES FOR GENERIC QUEUE
 *---------------------------------------------------------------------------
 */

/*
 *#===========================================================================
 *#                      QUEUE_init / SQUEUE_init
 *#===========================================================================
 * Description: Create and init queue.
 * Returns:     NONE
 */
static inline VOID QUEUE_init
(
	QUEUE_t			*pQueue		/* pointer to the queue structure       */
)
{
	pQueue->NumOfElements	= 0;
	pQueue->pHead			= (NODE_t*) NULL;
	pQueue->pTail			= (NODE_t*) NULL;
}
// See QUEUE_init
static inline VOID SQUEUE_init
(
	SQUEUE_t		*pQueue		/* The pointer to the queue structure       */
)
{
	pQueue->NumOfElements	= 0;
	pQueue->pHead			= (SNODE_t*) NULL;
	pQueue->pTail			= (SNODE_t*) NULL;
}

/*
 *#===========================================================================
 *#                      QUEUE_dequeue / SQUEUE_dequeue
 *#===========================================================================
 * Description: Release head
 * Returns:     pointer to the node, released from the head
 */
static inline NODE_t *QUEUE_dequeue
(
	QUEUE_t			*pQueue		/* The pointer to the queue structure       */
)
{
	NODE_t			*node = pQueue->pHead;
	if(pQueue->pHead)
	{
		pQueue->pHead = pQueue->pHead->pNext;
		pQueue->NumOfElements--;
		if(!pQueue->pHead)
		{
			/* The last element dequeued, queue is empty now */
			pQueue->pTail = (NODE_t*) NULL;
			ASSERT(pQueue->NumOfElements == 0);
		}
		else
		{
			pQueue->pHead->pPrevious = (NODE_t*)NULL;
		}
		node->pNext = (NODE_t*)NULL;
		node->pPrevious = (NODE_t*)NULL;
	}
	return (node);
}
// See QUEUE_dequeue
static inline SNODE_t *SQUEUE_dequeue
(
	SQUEUE_t		*pQueue		/* The pointer to the queue structure       */
)
{
	SNODE_t			*node = pQueue->pHead;
	if(pQueue->pHead)
	{
		pQueue->pHead = pQueue->pHead->pNext;
		pQueue->NumOfElements--;
		if(!pQueue->pHead)
		{
			/* The last element dequeued, queue is emty now */
			pQueue->pTail = (SNODE_t*) NULL;
			ASSERT(pQueue->NumOfElements == 0);
		}
		node->pNext = (SNODE_t*)NULL;
	}
	return (node);
}

/*
 *#===========================================================================
 *#               QUEUE_release_element / SQUEUE_release_element
 *#===========================================================================
 * Description: Release element
 * Returns:     NONE
 */
static inline VOID QUEUE_release_element
(
	QUEUE_t			*pQueue,	/* The pointer to the queue structure       */
	NODE_t			*pData		/* The pointer to the node to release       */
)
{
	if(pQueue->pHead == pData)
	{
		QUEUE_dequeue(pQueue);
	}
	else
	{
		ASSERT(pData->pPrevious);
		pData->pPrevious->pNext = pData->pNext;
		if(pData->pNext)
		{
			pData->pNext->pPrevious = pData->pPrevious;
		}
		else
		{
			pQueue->pTail = pData->pPrevious;
		}
		pQueue->NumOfElements--;
		pData->pPrevious = pData->pNext = (NODE_t*)NULL;
	}
}
// See QUEUE_release_element
static inline VOID SQUEUE_release_element
(
	SQUEUE_t		*pQueue,	/* The pointer to the queue structure       */
	SNODE_t			*pData		/* The pointer to the node to release       */
)
{
	if(pQueue->pHead == pData)
	{
		SQUEUE_dequeue(pQueue);
	}
	else
	{
		SNODE_t *p, *prev;
		for (prev = pQueue->pHead, p = prev->pNext; p != pData && p;
			p = p->pNext, prev = prev->pNext);
		if (p)
		{
			prev->pNext = p->pNext;
			if (!p->pNext) pQueue->pTail = prev;
			else p->pNext = (SNODE_t*)NULL;
			pQueue->NumOfElements--;
		}
	}
}



/*
 *#===========================================================================
 *#                      QUEUE_enqueue / SQUEUE_enqueue
 *#===========================================================================
 * Description: Enqueue element to the tail of the queue.
 * Returns:     NONE
 */
static inline VOID QUEUE_enqueue
(
	QUEUE_t			*pQueue,	/* The pointer to the queue structure       */
	NODE_t			*pData		/* The pointer to the node to enqueue       */
)
{
	pData->pNext				= (NODE_t*)NULL;
	pData->pPrevious			= pQueue->pTail;
	if (pQueue->pHead)
	{
		ASSERT(pQueue->pTail);
		pQueue->pTail->pNext	= pData;
	}
	else
	{
		pQueue->pHead			= pData;
	}
	pQueue->pTail				= pData;
	pQueue->NumOfElements++;
}

// See QUEUE_enqueue
static inline VOID SQUEUE_enqueue
(
	SQUEUE_t		*pQueue,	/* The pointer to the queue structure       */
	SNODE_t			*pData		/* The pointer to the node to enqueue       */
)
{
	pData->pNext				= (SNODE_t*)NULL;
	if (pQueue->pHead)
	{
		ASSERT(pQueue->pTail);
		pQueue->pTail->pNext	= pData;
	}
	else
	{
		pQueue->pHead			= pData;
	}
	pQueue->pTail				= pData;
	pQueue->NumOfElements++;
}


/*
 *#===========================================================================
 *#                      QUEUE_attach
 *#===========================================================================
 * Description: Attach element after a defined place in queue.
 * Returns:     NONE
 */
static inline VOID QUEUE_attach
(
	QUEUE_t			*pQueue,	/* The pointer to the queue structure       */
    NODE_t			*pNode,		/* The pointer to the node                  */
    NODE_t			*pData		/* The pointer to the node to enqueue       */
)
{
    if(!pData)
        return;

    /* if in the begining of the list*/
    if (!pNode)
    {
        pData->pPrevious = (NODE_t*)NULL;
        /*if the list is not empty*/
        if (pQueue->pHead)
        {
            pData->pNext             = pQueue->pHead;
            pQueue->pHead->pPrevious = pData;
        }
        else
        {
            pQueue->pTail = pData;
            pData->pNext  = (NODE_t*)NULL ;
        }

		pQueue->pHead = pData;
		pQueue->NumOfElements++;
    }
    else
    {
        /* if in the end of the list*/
        if  (pNode == pQueue->pTail)
        {
            QUEUE_enqueue(pQueue, pData);
        }
        else
        {
            pData->pNext        	= pNode->pNext;
            pData->pPrevious    	= pNode;
            pNode->pNext->pPrevious = pData;
            pNode->pNext 			= pData;
            pQueue->NumOfElements++;
        }
    }
}



/*
 *===========================================================================
 *                      SQUEUE_attach
 *===========================================================================
 * Description: Attach element after a defined place in queue.
 * Returns:     NONE
 */
static inline VOID SQUEUE_attach
(
	SQUEUE_t			*pQueue,	/* The pointer to the queue structure       */
    SNODE_t			*pNode,		/* The pointer to the node                  */
    SNODE_t			*pData		/* The pointer to the node to enqueue       */
)
{
    if(!pData)
        return;


    /* if in the begining of the list*/
    if (!pNode)
    {
        /*if the list is not empty*/
        if (pQueue->pHead)
        {
            pData->pNext             = pQueue->pHead;
        }
        else
        {
            pQueue->pTail = pData;
            pData->pNext  = (SNODE_t*)NULL ;
        }

		pQueue->pHead = pData;
		pQueue->NumOfElements++;
    }
    else
    {
        /* if in the end of the list*/
        if  (pNode == pQueue->pTail)
        {
            SQUEUE_enqueue(pQueue, pData);
        }
        else
        {
            pData->pNext        	= pNode->pNext;
            pNode->pNext 			= pData;
            pQueue->NumOfElements++;
        }
    }
}

/*
 *===========================================================================
 *                      QUEUE_enqueue_before
 *===========================================================================
 * Description: Attach element before a defined place in queue.
 * Returns:     NONE
 */
static inline VOID QUEUE_enqueue_before
(
	QUEUE_t			*pQueue,	/* The pointer to the queue structure       */
    NODE_t			*pNode,		/* The pointer to the node                  */
    NODE_t			*pData		/* The pointer to the node to enqueue       */
)
{
    if (!pData)
        return;

    /* if in the list is empty */
    if (!pNode)
    {
        QUEUE_enqueue(pQueue, pData);
    }
    else
    {
    	if(pQueue->pHead == pNode)
    	{
    		pQueue->pHead = pData;
    	}

        pData->pNext        	= pNode;
        pData->pPrevious    	= pNode->pPrevious;

		if(pNode->pPrevious)
		{
        	pNode->pPrevious->pNext	= pData;
		}
        pNode->pPrevious		= pData;
        pQueue->NumOfElements++;
    }
}

/*
 *#===========================================================================
 *#                      QUEUE_get_first / SQUEUE_get_first
 *#===========================================================================
 * Description: Get head element (first node).
 * Returns:     Pointer to the head node of the queue.
 */
static inline NODE_t *QUEUE_get_first
(
	QUEUE_t			*pQueue		/* The pointer to the queue structure       */
)
{
	return (pQueue->pHead);
}
// See QUEUE_get_first
static inline SNODE_t *SQUEUE_get_first
(
	SQUEUE_t		*pQueue		/* The pointer to the queue structure       */
)
{
	return (pQueue->pHead);
}

/*
 *#===========================================================================
 *#                      QUEUE_get_last / SQUEUE_get_last
 *#===========================================================================
 * Description: Get head element (last node).
 * Returns:     Pointer to the head node of queue.
 */
static inline NODE_t *QUEUE_get_last
(
	QUEUE_t			*pQueue		/* The pointer to the queue structure       */
)
{
	return (pQueue->pTail);
}
// See QUEUE_get_last
static inline SNODE_t *SQUEUE_get_last
(
	SQUEUE_t		*pQueue		/* The pointer to the queue structure       */
)
{
	return (pQueue->pTail);
}

/*
 *#===========================================================================
 *#                      QUEUE_get_next / SQUEUE_get_next
 *#===========================================================================
 * Description: Get next element.
 * Returns:     Pointer to the next element in the queue (next node).
 */
static inline NODE_t *QUEUE_get_next
(
	QUEUE_t			*pQueue,	/* The pointer to the queue structure       */
	NODE_t			*pData		/* The pointer to the current node          */
)
{
	UNUSED(pQueue);
	return (pData->pNext);
}

// See QUEUE_get_next.
static inline SNODE_t *SQUEUE_get_next
(
	SQUEUE_t		*pQueue,	/* The pointer to the queue structure       */
	SNODE_t			*pData		/* The pointer to the current node          */
)
{
	UNUSED(pQueue);
	return (pData->pNext);
}

/*
 *#===========================================================================
 *#                      QUEUE_get_previous
 *#===========================================================================
 * Description: Get previous element.
 * Returns:     Pointer to the previous element in queue (prev node).
 */
static inline NODE_t *QUEUE_get_previous
(
	QUEUE_t			*pQueue,	/* The pointer to the queue structure       */
	NODE_t			*pData		/* The pointer to the current node          */
)
{
	UNUSED(pQueue);
	return (pData->pPrevious);
}

/*
 *#===========================================================================
 *#                      QUEUE_concat / SQUEUE_concat
 *#===========================================================================
 * Description: Concatenate two queues and clears the 2nd queue.
 * Returns:     Pointer to the concatenated queue (the 1st queue).
 */
static inline QUEUE_t *QUEUE_concat
(
	QUEUE_t			*pQueue1,	/* The pointer to the 1st queue structure   */
	QUEUE_t			*pQueue2	/* The pointer to the 2nd queue structure   */
)
{
	NODE_t *pData				= pQueue2->pHead;
	if (!pData) return (pQueue1);
	pData->pPrevious			= pQueue1->pTail;
	if (pQueue1->pHead)
	{
		ASSERT(pQueue1->pTail);
		pQueue1->pTail->pNext	= pData;
	}
	else
	{
		pQueue1->pHead			= pData;
	}
	pQueue1->pTail				= pQueue2->pTail;
	pQueue1->NumOfElements		+= pQueue2->NumOfElements;
	QUEUE_init(pQueue2);
	return (pQueue1);
}

// See QUEUE_concat
static inline SQUEUE_t *SQUEUE_concat
(
	SQUEUE_t		*pQueue1,	/* The pointer to the 1st queue structure   */
	SQUEUE_t		*pQueue2	/* The pointer to the 2nd queue structure   */
)
{
	SNODE_t *pData				= pQueue2->pHead;
	if (!pData) return (pQueue1);
	if (pQueue1->pHead)
	{
		ASSERT(pQueue1->pTail);
		pQueue1->pTail->pNext	= pData;
	}
	else
	{
		pQueue1->pHead			= pData;
	}
	pQueue1->pTail				= pQueue2->pTail;
	pQueue1->NumOfElements		+= pQueue2->NumOfElements;
	SQUEUE_init(pQueue2);
	return (pQueue1);
}
/*
 *#===========================================================================
 *#                      QUEUE_detach / SQUEUE_detach
 *#===========================================================================
 * Description: Detach number of elements from queue
 * Returns:     Pointer to the first item
 */
static inline NODE_t *QUEUE_detach
(
	QUEUE_t			*pQueue,	/* The pointer to the queue structure   */
	UINT16			 NumOfElements	/*Number of elements to detach*/
)
{
	NODE_t			*head = pQueue->pHead, *node = head;
	ASSERT(NumOfElements);
	ASSERT(pQueue->NumOfElements >= NumOfElements);
 	while (NumOfElements--)
	{
		node = pQueue->pHead;
		ASSERT(node);
		pQueue->pHead = pQueue->pHead->pNext;
		pQueue->NumOfElements--;
		if(!pQueue->pHead)
		{
			/* The last element dequeued, queue is empty now */
			pQueue->pTail = (NODE_t*) NULL;
			ASSERT(pQueue->NumOfElements == 0);
		}
		else
		{
			pQueue->pHead->pPrevious = (NODE_t*)NULL;
		}
	}
	head->pPrevious = (NODE_t*)NULL;
 	node->pNext = (NODE_t*)NULL;
 	return (head);
}

// See QUEUE_detach
static inline SNODE_t *SQUEUE_detach
(
	SQUEUE_t		 *pQueue,	/* The pointer to the queue structure   */
	UINT16			 NumOfElements	/*Number of elements to detach*/
)
{
	SNODE_t			*head = pQueue->pHead, *node = head;
	ASSERT(NumOfElements);
	ASSERT(pQueue->NumOfElements >= NumOfElements);
	while (NumOfElements--)
	{
		node = pQueue->pHead;
		ASSERT(node);
		pQueue->pHead = pQueue->pHead->pNext;
		pQueue->NumOfElements--;
		if(!pQueue->pHead)
		{
			/* The last element dequeued, queue is emty now */
			pQueue->pTail = (SNODE_t*) NULL;
			ASSERT(pQueue->NumOfElements == 0);
		}
	}
 	node->pNext = (SNODE_t*)NULL;
 	return (head);
}

/*
 *#===========================================================================
 *#                      QUEUE_isHead
 *#===========================================================================
 * Description: Return TRUE or FALSE if node is head of queue.
 * Returns:     BOOLEAN
 */
static inline BOOL QUEUE_isHead
(
	QUEUE_t			*pQueue,	/* The pointer to the queue structure       */
    NODE_t			*pData		/* The pointer to the node to enqueue       */
)
{
	if(pData)
	{
		if (pQueue->pHead == pData)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

#ifdef __cplusplus
}
#endif

#endif /*_I_QUEUE_H*/


/*
 *---------------------------------------------------------------------------
 *                      END OF FILE
 *---------------------------------------------------------------------------
 */
