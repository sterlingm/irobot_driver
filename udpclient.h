#ifndef UDPCLIENT_H
#define UDPCLIENT_H


class Agent;

class udpclient {
public:

    udpclient(char*);
    ~udpclient();

    bool launch_client();
    void communicate();

    void setAgent(Agent*);
    Agent*& getAgent();
private:
    int fd;
    char* port;
    Agent* myAgent;
};

#endif
