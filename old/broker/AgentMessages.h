#ifndef AGENT_MESSAGES_H
#define AGENT_MESSAGES_H


namespace Madara
  {
    class AgentContext;

    enum size_t
      {
        AGENT_PING = 1,
        AGENT_DISCOVER = 2,
        AGENT_DISCOVER_HOST = 3,
        AGENT_LATENCY_QUERY_RESPONSE = 5,
        AGENT_LATENCY_DESCRIBE = 10,
        AGENT_DUMP_CONTEXT = 20,
        BROKER_LATENCY_QUERY = 100,
        BROKER_DEPLOYMENT_OFFER = 101,
        BROKER_DEPLOYMENT_PRINT = 102
      };

    typedef struct
    {
      unsigned int size;
      unsigned int type;
      char host[16];
      unsigned int port;
    } AgentPing;

    typedef struct
    {
      char host[16];
      unsigned int port;
      unsigned int latency;
    } AgentLatency;

    typedef struct
    {
      char name[256];
    } AgentFile;

    typedef struct
    {
      unsigned int rank;
      char host[16];
      unsigned int port;
    } AgentDeploymentRank;

    void fillPingFromContext (AgentPing & ping, 
                              AgentContext & context);
  }
#endif