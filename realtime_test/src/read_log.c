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
    gnuplot_setstyle(handle, "lines");
    sleep(1);

    int n = file_size/data_size;
    double t[n];
    double lon[n];
    double lat[n];
    double hour[n];
    double min[n];
    double sec[n];
    int nano[n];

    printf("----------starting file read---------------\n");
    int i = 0;
    while(i<n){
        r = fread(data, data_size, 1, fp_log);

        struct timespec timestamp;
        timestamp.tv_sec = (long)*(data);
        timestamp.tv_nsec = (long)*(data+8);
        Sensor_Error_Typedef se = sensor_process_data(data + 16, data_size - 16);
        if(!se){
            //print_GNSS_data();
            t[i] =  i*4; //timestamp.tv_nsec; //i*4; //ms
            lon[i] = (double)Global_GNSS_Data.GNSS_lon*1e-7; //Global_Sensor_Data.accX;
            lat[i] = (double)Global_GNSS_Data.GNSS_lat*1e-7;//Global_Sensor_Data.accY;
            hour[i] = (double)Global_GNSS_Data.GNSS_hour;//Global_Sensor_Data.accZ;
            min[i] = (double)Global_GNSS_Data.GNSS_min;
            sec[i] = (double)Global_GNSS_Data.GNSS_sec;
            nano[i] = Global_GNSS_Data.GNSS_nano;

            //printf("%f \t %f \t %f \t %f \n", t[i], x[i], y[i], z[i]);

            if(nano[i] != nano[i-1]){
                printf("%d \t||\t %d:%d:%2d:%9d \t %f:%f \n", (int)t[i], (int)hour[i], (int)min[i], (int)sec[i], nano[i], lon[i], lat[i]);

            }
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
    gnuplot_plot_xy(handle, t, lon, n, "x") ;
    gnuplot_plot_xy(handle, t, lat, n, "x") ;

    printf("-------------done----------------\n");

    gnuplot_close(handle);

    return 0;
}