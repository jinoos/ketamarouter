#ifndef _KROUTER_H_
#define _KROUTER_H_

// #define MAX_RECENT_COUNTER_SECONDS	10

enum NODE_STAT
{
    NODE_STAT_STARTING,     // after starting node application, but not started yet.
    NODE_STAT_READY,        // finished bootup application, ready to serve
    NODE_STAT_RUNNING,      // the node serves service to client
    NODE_STAT_SUSPENDING,   // checked abnormal signal on the node
    NODE_STAT_SUSPENDED,    // removed service node list
    NODE_STAT_RESUMING,     // returning back to normal RUNNING status
    NODE_STAT_SHUTDOWNING,  // shutdowning node, not in service node
    NODE_STAT_SHUTDOWN      // node is down.
};

#endif // _KROUTER_H_
