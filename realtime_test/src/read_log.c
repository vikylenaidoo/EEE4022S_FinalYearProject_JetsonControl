/**
 * for reading logged data and plotting graph
 * reference gnuplot library: http://ndevilla.free.fr/gnuplot/gnuplot_i/index.html
*/

#include "tasks/sensor_handler.h"
#include "lib/gnuplot_i.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
//#include <time.h>
#include <unistd.h>

#define data_size   148

int main(int argc, char *argv[]){

    //setup file
    FILE* fp_log = fopen("data/data_log", "rb");
    if(fp_log == NULL){
        printf("failed open file\n");
        return 1;
    }
    struct stat st;
    fstat(fileno(fp_log), &st);
    int file_size = st.st_size;

    //array to store read data
    uint8_t data [data_size];
    int r = 1;

    //setup gnuplot
    gnuplot_ctrl *handle;
    handle = gnuplot_init();
    gnuplot_setstyle(handle, "points");
    sleep(1);

    int n = file_size/data_size;
    double t[n];
    double x[n];
    int y[n];
    int z[n];

    printf("----------startinf file read---------------\n");
    int i = 0;
    while(i<n){
        r = fread(data, data_size, 1, fp_log);

        struct timespec timestamp;
        timestamp.tv_sec = (long)*(data);
        timestamp.tv_nsec = (long)*(data+8);
        Sensor_Error_Typedef se = sensor_process_data(data + 16, data_size - 16);
        if(!se){
            //print_GNSS_data();
            t[i] = i;//timestamp.tv_nsec;
            x[i] = Global_GNSS_Data.GNSS_lat*1e-7;//Global_Sensor_Data.accX;
            y[i] = Global_Sensor_Data.accY;
            z[i] = Global_Sensor_Data.accZ;
        }
        else{
            printf("read error");
            return 1;
        }
        //printf("sec:nsec %ld %ld \n", timestamp.tv_sec, timestamp.tv_nsec);

        i++;
    }

    printf("-------------plot graphs----------------\n");
    //plot graph
    gnuplot_cmd(handle, "set terminal png");
	gnuplot_cmd(handle, "set output \"log_graph.png\"");
    gnuplot_plot_xy(handle, t, x, n, "x Accelrations") ;

    printf("-------------done----------------\n");

    gnuplot_close(handle);

    return 0;
}