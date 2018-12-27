
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>


#include "config.h"

extern int simm_mme_init();
extern int imm_mme_setup();


topology_t g_topology = {
    .ports[SIM_MME].name = "sim_mme",
    .ports[SIM_MME].peer = CP_S11,

    .ports[SIM_S1U].name = "sim_s1u",
    .ports[SIM_S1U].peer = DP_S1U,

    .ports[CP_S11].name = "cp_s11",

    .ports[DP_S1U].name = "dp_s1u",
    .portCount = DP_SGI,

};

g_config_t g_config = {
    .numberOfUEs = 1,

};

char g_ethernetList[20][48];
int  g_ethernetListCount = 0;


//! 
//!
//! \author root (12/13/18)
//! \brief use sysFS to get all ethernet ports, even those without IP addresses set
//! \return int number of ethernet ports found
int ph_updateEthernetList(){
    DIR *d;
    struct dirent *dir;

    d = opendir("/sys/class/net");
    g_ethernetListCount = 0;

    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if(strcmp(".",dir->d_name) == 0) continue;
            if(strcmp("..",dir->d_name) == 0) continue;
            printf("%s\n", dir->d_name);
            strcpy(g_ethernetList[g_ethernetListCount], dir->d_name);
            g_ethernetListCount++;
        }
        closedir(d);
    }
    return g_ethernetListCount;
}

//! 
//!
//! \author root (12/13/18)
//!
//! \param name ethernet port name to look for
//!
//! \return int o not found, 1 found
int ph_findErnernetByName(char *name){
    int i = 0;
    for (; i < g_ethernetListCount; i++) {
        if (strcmp(name, g_ethernetList[i]) == 0) {
            return 1;
        }
    }
    return 0;

}

int main( int argc, const char* argv[] )
{
    int i, j;
    char systemCommand[64];
    FILE *fp,*outputfile;
    char var[40];
    //build initial ethernet device list
    //keep in g_ethernetList
    ph_updateEthernetList();
/*
    if (ph_findErnernetByName("veth0")) {
        printf("veth0 found\n");
    }
*/

    //check if veth pairs have already been built and configured correctly
    for (i = 0; i < g_topology.portCount; i++) {
        if (g_topology.ports[i].name != NULL) {
            if (strncmp("sim_", g_topology.ports[i].name, 4) == 0) {
                if (ph_findErnernetByName(g_topology.ports[i].name)) {
                    //found
                    printf( "%s found\n", g_topology.ports[i].name);
                    

                }
                else{
                    //need to create it
                    sprintf(systemCommand, "ip link add %s type veth peer name %s",
                            g_topology.ports[i].name,
                            g_topology.ports[g_topology.ports[i].peer].name);
                    if (system(systemCommand)) {
                        printf("failed to create veth %s\n", g_topology.ports[i].name);
                        return -1;
                    }
                    else {
                        printf("created veth %s<++>%s\b",
                               g_topology.ports[i].name,
                               g_topology.ports[g_topology.ports[i].peer].name);
                    }

                }
 
                 //bring both ends up
                // ip  link set vmh1 up
                sprintf(systemCommand, "ip link set %s up",
                         g_topology.ports[i].name);
                system(systemCommand);
                sprintf(systemCommand, "ip link set %s up",
                         g_topology.ports[g_topology.ports[i].peer].name);
                system(systemCommand);

            }
            else {
                printf( "%s found\n", g_topology.ports[i].name);
            }
            //get mac address
             sprintf(systemCommand,"cat /sys/class/net/%s/address", g_topology.ports[i].name);

             fp = popen(systemCommand, "r");
             if (fgets(var, sizeof(var), fp) != NULL) 
             {
                 printf("%s", var);
                 //parse into g_topology.ports
                 for (j=0; j<6; j++) {
                     g_topology.ports[i].mac[j] = var[j*3 +1] < 'a' ? var[j*3 +1] - '0': var[j*3 +1] - 'a' +10;
                     g_topology.ports[i].mac[j] |= var[j*3 ] < 'a' ?( var[j*3] - '0') << 4: (var[j*3] - 'a' +10) <<4;
                     printf("%02x:",g_topology.ports[i].mac[j] );
                 }
                 printf("\n");
             }
             pclose(fp);
        }
    }


 simm_mme_init();
 imm_mme_setup();

	printf( "\nHello World\n\n" );
    return 0;
}
