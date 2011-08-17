#ifndef UDPSERVER_H
#define UDPSERVER_H


class Agent;

class udpserver {
public:
    udpserver(char*);
    ~udpserver();

    void setAgent(Agent*);
    Agent*& getAgent();

    bool launch_server();
    void get_message(char*);
    void communicate();

private:
    int fd;
    char* port;
    Agent* myAgent;
};

#endif
