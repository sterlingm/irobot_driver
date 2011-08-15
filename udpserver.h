#ifndef UDPSERVER_H
#define UDPSERVER_H


class Agent;

class udpserver {
public:
    udpserver(char*);
    ~udpserver();

    bool launch_server();
    void communicate();

    void setAgent(Agent*);
    Agent*& getAgent();

private:
    void get_message(char*);


    int fd;
    char* port;
    Agent* myAgent;
};

#endif
