#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>
#include <linux/types.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define MAX_SLEEP_TIME 5
#define MAX_PASENGERS 300

static bool exit_dispatcher;

static void handler(int signum) {
    exit_dispatcher = true;
}

int plane(int num_passengers) {
    int landing_strip;
    __u64 plane_id;

    plane_id = getpid();
    printf("Plane: id %ld; Passengers %d\n",plane_id,num_passengers);

    landing_strip = open("/dev/airport_land_strip",O_RDWR);
    if (landing_strip == -1) {
        printf("Plane: id %d, failed to open airport_land_strip, errno %d, desc %s\n",plane_id,errno,strerror(errno));
        return 1;
    }    

    
    if (close(landing_strip) == -1) {
         printf("Plane: id %d, failed to close airport_land_strip, errno %d, desc %s\n",plane_id,errno,strerror(errno));
        return 1; 
    }
    return 0;
}

int main(int argc, char* argv[]) {
    int proc_status; 
    pid_t child_id;
    struct sigaction sa;

    exit_dispatcher = false;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        printf("Could not register signal handler, exiting ...\n");
        return 1;
    }

    while (!exit_dispatcher) {
        if ((child_id=fork()) == 0) {
            plane(rand()%MAX_PASENGERS);        
            exit(0);
        } else {
            sleep(rand()%MAX_SLEEP_TIME);
        }
    }

    /* wait for all processes to exit */
    printf("Waiting for planes to leave\n");
    while((wait(&proc_status)!=-1) && (errno!=ECHILD))
        ;
    printf("Exiting ... \n");
    return 0;
}
