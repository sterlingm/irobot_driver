#ifndef UDPCLIENT_H
#define UDPCLIENT_H


class Agent;

class udpclient {
public:

    udpclient(char*);
    ~udpclient();

    void setAgent(Agent*);
    Agent*& getAgent();

    bool launch_client();
    void communicate();

private:
    int fd;
    char* port;
    Agent* myAgent;
};

#endif
