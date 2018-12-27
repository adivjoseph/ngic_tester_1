
typedef struct topology_port_s {
    char *name;
    int peer;
    char mac[6];
    int  ip;
}topology_port_t;


#define MAX_TOPOLOGY_PORTS  10
enum tport{
    SIM_MME,
    SIM_S1U,
    SIM_SGI,
    CP_CTL,
    CP_S11,
    DP_CTL,
    DP_S1U,
    DP_SGI,
};

typedef struct topology_s {
    topology_port_t ports[MAX_TOPOLOGY_PORTS];
    int portCount;

} topology_t;


typedef struct g_config_s {
    int numberOfUEs;        //numer of bearers to enable

}g_config_t;
