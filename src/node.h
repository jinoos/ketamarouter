#ifndef _NODE_H_
#define _NODE_H_

#define NODE_ADDR_SIZE   255

typedef struct node_t
{
    char addr[NODE_ADDR_SIZE];
    int port;
    unsigned long weight;
} node_t;

#endif // _NODE_H_
