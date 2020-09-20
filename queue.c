#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "harness.h"
#include "queue.h"

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (!q)
        return NULL;
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (!q) {
        return;
    }

    while (q->head) {
        list_ele_t* rm_node = q->head;
        q->head = q->head->next;
        free(rm_node->value);
        free(rm_node);
    }
    free(q);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    list_ele_t *newh;

    /*
     * DO NOT do this check after malloc
     * Memory leak is prone to happen
     */
    if (!q) {
        return false;
    }

    newh = malloc(sizeof(list_ele_t));
    if (!newh) {
        return false;
    }
    newh->next = NULL;
    newh->value = NULL;

    /*
     * Extra 1 bytes for '\0'
     * Is calling strlen(s) bad for performance ??
     */
    size_t copy_size = (sizeof(char) * strlen(s)) + (sizeof(char) * 1);
    newh->value = (char*) malloc(copy_size);
    if (!newh->value) {
        /* Failed to construct newh, free it! */
        free(newh);
        return false;
    }
    memcpy(newh->value, s, copy_size);

    if (!q->head) {
        q->head = q->tail = newh;
    } else {
        newh->next = q->head;
        q->head = newh;
    }
    q->size += 1;
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    list_ele_t* newt;

    if (!q) {
        return false;
    }

    newt = malloc(sizeof(list_ele_t));
    if (!newt) {
        return false;
    }
    newt->next = NULL;
    newt->value = NULL;

    /*
     * Extra 1 bytes for '\0'
     * Is calling strlen(s) bad for performance ??
     */
    size_t copy_size = (sizeof(char) * strlen(s)) + (sizeof(char) * 1);
    newt->value = (char*) malloc(copy_size);
    if (!newt->value) {
        /* Failed to construct newt, free it! */
        free(newt);
        return false;
    }
    memcpy(newt->value, s, copy_size);

    if (!q->tail) {
        q->head = q->tail = newt;
    } else {
        q->tail->next = newt;
        q->tail = newt;
    }
    q->size += 1;
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (!q || !q->head) {
        return false;
    }

    list_ele_t* rm_node = q->head;
    q->head = q->head->next;
    q->size -= 1;
    if (!q->head) {
        q->tail = NULL;
    }

    size_t copy_size = (sizeof(char) * strlen(rm_node->value)) + (sizeof(char) * 1);
    if (sp)
    {
        memcpy(sp, rm_node->value, copy_size <= bufsize ? copy_size : bufsize);
        sp[bufsize-1] = '\0';
    }

    free(rm_node->value);
    free(rm_node);
    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    if (!q)
        return 0;
    return q->size;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (!q || !q->head) {
        return;
    }

    q->tail = q->head;
    
    list_ele_t* cur = q->head->next;
    while (cur) {
        list_ele_t* prev = q->head;
        q->head = cur;
        cur = cur->next;
        q->head->next = prev;
    }
    q->tail->next = NULL;
}

/*
 * Sort linked list
 * Return the head of linked list
 */
static list_ele_t* q_sort_mergesort(list_ele_t* p1)
{
    if (!p1->next)
        return p1;
    list_ele_t* stp1 = p1;
    list_ele_t* stp2 = p1->next;
    while (stp2 && stp2->next) {
        stp1 = stp1->next;
        stp2 = stp2->next->next;
    }
    list_ele_t* p2 = stp1->next;
    stp1->next = NULL;

    p1 = q_sort_mergesort(p1);
    p2 = q_sort_mergesort(p2);

    /* make *p1 < *p2, i.e., make p1 be the head of list */
    if (strcmp(p1->value, p2->value) > 0) {
        list_ele_t* tmp = p1;
        p1 = p2;
        p2 = tmp;
    }
    list_ele_t* ret_head = p1;
    list_ele_t* p3 = p1->next;

    /* p1 is the smallest among the three, p2 or p3 is p1's successor */
    while (p2 && p3) {
        int res = strcmp(p2->value, p3->value);
        if (res < 0) {
            p1->next = p2;
            p1 = p2;
            p2 = p2->next;
        }
        else {
            p1->next = p3;
            p1 = p3;
            p3 = p3->next;
        }
    }
    p1->next = p2 ? p2 : p3;

    return ret_head;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    if (q_size(q) <= 1)
        return;

    q->head = q_sort_mergesort(q->head);

    /* O(n) reconstruct q->tail */
    while (q->tail->next)
    {
        q->tail = q->tail->next;
    }
}


// void merge_sort(list_ele_t **head)
// {
//     if (!(*head) || !((*head)->next))
//         return;

//     list_ele_t *l1 = (*head)->next;  
//     list_ele_t *l2 = *head;          

//     while (l1 && l1->next) {
//         l2 = l2->next;
//         l1 = l1->next->next;
//     }
//     l1 = l2->next;
//     l2->next = NULL;
//     l2 = *head;

//     merge_sort(&l2);
//     merge_sort(&l1);

//     *head = NULL;
//     list_ele_t **tmp = head;

//     while (l1 && l2) {
//         if (strcmp(l1->value, l2->value) < 0) {  
//             *tmp = l1;
//             l1 = l1->next;
//         } else {
//             *tmp = l2;
//             l2 = l2->next;
//         }
//         tmp = &((*tmp)->next);
//     }

//     *tmp = l1 ? l1 : l2;
// }

// void q_sort(queue_t *q)
// {
//     // if q has only one element or q is empty, q->head == q->tail
//     if (!q || q->head == q->tail) {
//         return;
//     }

//     merge_sort(&q->head);

//     while (q->tail->next) {
//         q->tail = q->tail->next;
//     }
// }