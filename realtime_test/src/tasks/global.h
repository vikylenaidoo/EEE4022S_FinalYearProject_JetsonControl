//------------------INCLUDES------------------------//



//----------------------DEFINES----------------------//

typedef struct{

    //acceleration data
    int16_t accX;      //12 bits        //2s complement
    int16_t accY;      //12 bits
    int16_t accZ;      //12 bits

    //gyro data
    int16_t gyroX;     //16 bits        //2s complement
    int16_t gyroY;     //16 bits 
    int16_t gyroZ;     //16 bits

    //magneto data
    int16_t magX;      //13 bits       //2s complement
    int16_t magY;      //13 bits
    int16_t magZ;      //15 bits
    uint16_t magHall;  //14 bits       //unsigned

    //baro data
    uint32_t baroPress;  //20 bits      //unsigned
    uint32_t baroTemp;   //20 bits      //unsigned
    

} Sensor_Data_Typedef;

/*
*contains message UBX-NAV-PVT
*Navigation position velocity time solution
*see neo-m8t interface guide for more details
*/
typedef struct{
    //type      name            unit            description
    //---------------------------------------------------
    uint32_t    GNSS_iTow;      //ms            GPS time of week of the navigation epoch.
    uint16_t    GNSS_year;      //year          year(utc)
    uint8_t     GNSS_month;     //month         Month, range 1-12 (UTC)
    uint8_t     GNSS_day;       //day           Day of month, range 1..31 (UTC)
    uint8_t     GNSS_hour;      //hour          Hour of day, range 0..23 (UTC)
    uint8_t     GNSS_min;       //min           Minute of hour, range 0..59 (UTC)
    uint8_t     GNSS_sec;       //s             Seconds of minute, range 0..60 (UTC)
    uint8_t     GNSS_valid;     //              Validity flags 
                                                /*bit3=validMag, 
                                                bit2=fullyResolved, 
                                                bit1=validTime, 
                                                bit0=validDate*/
    uint32_t    GNSS_tAcc;      //ns            Time accuracy estimate (UTC)
    int32_t     GNSS_nano;      //ns            Fraction of second, range -1e9 .. 1e9 (UTC)
    uint8_t     GNSS_fixType;   //              GNSSfix Type:
                                                /*0: no fix
                                                1: dead reckoning only
                                                2: 2D-fix
                                                3: 3D-fix
                                                4: GNSS + dead reckoning combined
                                                5: time only fix */
    uint8_t     GNSS_flags1;    /*              bit7-6: carrSoln                                                
                                                bit5:   headVehValid
                                                bit4-2: psmState
                                                bit1:   diffSoln
                                                bit0:   gnssFixOK   */
    uint8_t     GNSS_flags2;    /*              bit7:   confirmedTime
                                                bit6:   confirmedDate
                                                bit5:   confirmedAvai   */
                                                                                                
    uint8_t     GNSS_numSV;     //              Number of satellites used in Nav Solution
    int32_t     GNSS_lon;       //deg (1e-7)    longitude
    int32_t     GNSS_lat;       //deg (1e-7)    latitude
    int32_t     GNSS_height;    //mm            height above ellipsoid
    int32_t     GNSS_hMSL;      //mm            height above mean sea level
    uint32_t    GNSS_hAcc;      //mm            horizontal accuracy estimate
    uint32_t    GNSS_vAcc;      //mm            vertical accuracy estimate
    int32_t     GNSS_velN;      //mm/s          NED north velocity
    int32_t     GNSS_velE;      //mm/s          NED east velocity
    int32_t     GNSS_velD;      //mm/s          NED down velocity
    int32_t     GNSS_gSpeed;    //mm/s          ground speed (2d)
    int32_t     GNSS_headMot;   //deg (1e-5)    heading of motion(2d)
    uint32_t    GNSS_sAcc;      //mm/s          Speed accuracy estimate
    uint32_t    GNSS_headAcc;   //deg (1e-5)    Heading accuracy estimate (both motion and vehicle)
    uint16_t    GNSS_pDOP;      // (0.01)       position DOP
    uint8_t     GNSS_flags3;    /*              bit0: invalidLlh    */
    uint8_t     GNSS_reserved1; //              ignore 
    uint8_t     GNSS_reserved2; //              ignore
    uint8_t     GNSS_reserved3; //              ignore
    uint8_t     GNSS_reserved4; //              ignore
    uint8_t     GNSS_reserved5; //              ignore
    int32_t     GNSS_headVeh;   //deg (1e-5)    Heading of vehicle (2-D), this is only valid when headVehValid is set, otherwise the output is set to the heading of motion
    int16_t     GNSS_magDec;    //deg (1e-2)    Magnetic declination. Only supported in ADR 4.10 and later.
    uint16_t    GNSS_magAcc;    //deg (1e-2)    Magnetic declination accuracy. Only supported in ADR 4.10 and later


} GNSS_Data_Typedef;

typedef enum{
    SENSOR_OK=0,
    SENSOR_LENGTH,
    SENSOR_READ,
    SENSOR_DATA_INVALID
} Sensor_Error_Typedef;

typedef enum{
    CRC32_OK=0,
    CRC32_FAIL

} CRC32_Error_Typedef;

//---------------------GLOBAL VARS---------------------//

Sensor_Data_Typedef Global_Sensor_Data;
GNSS_Data_Typedef Global_GNSS_Data;