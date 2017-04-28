/*
************************************************************************************************************************
*                                         ADT AEC
*
*            (c) Copyright 2016-2017; Guangzhou Guangyou Communication Equipment Co., Ltd.
*            Address: No.173, Dunhe Road, Haizhu, Guangzhou, Guangdong, China
*
*            All rights reserved.  Protected by international copyright laws.
*
*
* File      : MAIN.CPP
* Created   : Zheng Yu
* Modified  : Lu Huadong
* Version   : V0.0.12
*
* DESCRIPTION:
* ---------------
*           Acoustic processing and echo control Sound pickup and playback is an important area of
*           multimedia processing. This project depend on acoustics echo canceling algorithm library,
*           which from the AD (Analog Devices). Here, we use cross compile tool chain, called poky.
*           Because the target 'adt_aec' executable program will be run on SMARC-IMX6Q platform.
*
************************************************************************************************************************
*/

#include <QCoreApplication>
#include <QSettings>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <sys/time.h>
#include <fcntl.h>   /* added for GPIO */
#include <stdio.h>

#include <alsa/asoundlib.h>
#include "adt_typedef_user.h"
#include "iaecg4.h"

/***************************************************************************
 *
 * Global variables
 *
 ***************************************************************************/

//#define ADT_AEC_VERSION "0.0.11"

#define VER_MAJOR 0
#define VER_MINOR 0
#define VER_PATCH 12

// modified by luhuadong at 20170323
//#define FRAME_SIZE 	160
#define FRAME_SIZE 	80
#define MAX_FRAME_SIZE 640
#define SAMPLE_RATE	8000
#define EC_DUMP 1
#define PLAYBACK
// modified by luhuadong at 20170323
//#define PRINT_ERL

// Definition of application's audio I/O information.
typedef struct
{
    snd_pcm_t *pcm_handle_capture;
    snd_pcm_t *pcm_handle_playback;
} audioInfo_t;

typedef enum
{
    PCMALSA,
    PCMFILE,
    PCMRTP
} type_pcm_t;

typedef struct ADT_AECState {
    audioInfo_t audioInfo;          // audio I/O information
    IAECG4_Handle hAEC;

    bool is_enable;
    int by_pass_mode;
    type_pcm_t pcm_src;
    type_pcm_t pcm_dest;
#ifdef EC_DUMP
    int is_ec_dump;
    FILE *echofile;
    FILE *reffile;
    FILE *stereofile;
    FILE *cleanfile;
#endif
    bool gpio_enable;
    int  gpio_number;
} ADT_AECState;

struct pollfd fds[1];   // GPIO value file handlers
static int gpio_fd ;    //,gpio_fd2
static ADT_AECState g_AECState;

static int gpioInit(void);
static void gpioHandle(void);

static void init();
static void parseConfigFile();
static int  initAECEngine();
static bool initAudio(bool outputOnly, char *captureDev, char *playbackDev);
static bool initALSAPlayback(bool outputOnly, char *playbackDev);
static bool initALSACapture(char *captureDev);

static bool shutdownAECEngine();
static bool shutdownAudio();

static void getInputFrame(ADT_Int16 *ref, ADT_Int16 *echo);
static void outPutFrame(ADT_Int16 *clean);
static void frameProcess(void *ptr);
static void backGroundThread(void *ptr);

static int  xrunRecovery(snd_pcm_t *handle, int error);


/*
*******************************************************************************
*                         customMessageHandler
*
* Description: qqDebug logfile handle.
*
* Arguments  : argc         count of parameters
*
*              argv         parameters
*
*
* Returns    : == 0         Succeed.
*              <  0         Failed.
*******************************************************************************
*/
void  customMessageHandler(QtMsgType type, const char *msg)
{
    QString txt;

    switch (type) {
    case QtDebugMsg:
        txt = QString("qDebug: %1").arg(msg);
        break;
    case QtWarningMsg:
        txt = QString("Warning: %1").arg(msg);
        break;
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
        break;
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
        abort();
    }

    QString current_date_time = QDateTime::currentDateTime().toString("MMdd hh:mm:ss");
    QString message = QString("[%1]******** %4").arg(current_date_time).arg(txt);

    QFile outFile(QCoreApplication::applicationDirPath() +"/adt_aec.log");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << message << endl;
    outFile.flush();
    outFile.close();
}

/*
*******************************************************************************
*                                gpioInit
*
* Description: This function is the entry of program.
*
* Arguments  : argc         count of parameters
*
*              argv         parameters
*
*
* Returns    : == 0         Succeed.
*              <  0         Failed.
*******************************************************************************
*/
static int gpioInit(void)
{
    int ret;
    char ch;
    int fd;

    char path_gpio_direction[64];
    char path_gpio_edge[64];
    char path_gpio_value[64];

    sprintf(path_gpio_direction, "/sys/class/gpio/gpio%d/direction", g_AECState.gpio_number);
    sprintf(path_gpio_edge, "/sys/class/gpio/gpio%d/edge", g_AECState.gpio_number);
    sprintf(path_gpio_value, "/sys/class/gpio/gpio%d/value", g_AECState.gpio_number);

    fd = open(path_gpio_direction, O_RDWR);
    if(fd < 0) {
        qDebug("Open file hands free gpio (direction) failed!\n");
        return -1;
    }
    write(fd, "in", 2);
    close(fd);

    fd = open(path_gpio_edge, O_RDWR);
    if(fd < 0) {
        qDebug("Open file hands free gpio (edge) failed!\n");
        return -1;
    }
    write(fd, "both", 4);
    close(fd);

    gpio_fd = open(path_gpio_value, O_RDONLY);
    if(gpio_fd < 0) {
        qDebug("Open file hands free gpio (value) failed!\n");
        return -1;
    }

    fds[0].fd = gpio_fd;
    fds[0].events  = POLLPRI;

    ret = poll(fds, 1, 0);   /* first poll to clean residue r-event */
    if(ret == -1) {
        qDebug("Poll on gpio value files failed!\n");
        close(fds[0].fd);
        return -1;
    }
    else if(ret > 0) {
        qDebug("First poll return is :%d.\n", ret);
    }

    if(fds[0].revents & POLLPRI)
    {
        ret = lseek(gpio_fd, 0, SEEK_SET);
        if(ret == -1)
        {
            qDebug("errror lseek on fd2.\n");
            return -1;
        }
        ret = read(gpio_fd, &ch, 1);
        if(ret == -1)
        {
            qDebug("errror read on fd2.\n");
            return -1;
        }
        //        if(ch == 48)
        //        {
        //            g_AECState.is_enable=true;
        //        }
        //        else if(ch == 49)
        //        {
        //            g_AECState.is_enable=false;
        //        }
        qDebug("First value of CTRL pin is: %c. \n", ch);
    }

    g_AECState.is_enable = false;
    return 0;
}

/*
*******************************************************************************
*                               gpioHandle
*
* Description: This function is the entry of program.
*
* Arguments  : argc         count of parameters
*
*              argv         parameters
*
*
* Returns    : == 0         Succeed.
*              <  0         Failed.
*******************************************************************************
*/
static void gpioHandle(void)
{
    int ret;
    char ch;

    ret = poll(fds, 1, 0);   /* poll on 1 file and no wait */

    if(-1 == ret) {
        qDebug() << "gpioHandle : Poll on gpio value files failed!";
        //return;
    }
    else if(0 == ret) {
        //qDebug() << "gpioHandle : Poll nothing, bye";
        //return;
    }
    else if(ret > 0) {
        qDebug() << "gpioHandle : Poll ready, ret = " << ret;

        if(fds[0].revents & POLLPRI)
        {
            ret = lseek(gpio_fd, 0, SEEK_SET);
            if(ret == -1) {
                qDebug("errror lseek on gpio_fd5.\n");
                return;
            }

            ret = read(gpio_fd, &ch, 1);
            if(ret == -1) {
                qDebug("errror read on fd.\n");
                return;
            }

            if('0' == ch) {
                g_AECState.is_enable = true;
                qDebug("Hands free GPIO value is 0. AEC is enable.");
            }
            else if ('1' == ch) {
                g_AECState.is_enable = false;
                qDebug("Hands free GPIO value is 1. AEC is disable.");
            }
        } // End if(fds[0].revents & POLLPRI)
    }

    return;
}

/*
*******************************************************************************
*                            parseConfigFile
*
* Description: This function is the entry of program.
*
* Arguments  : argc         count of parameters
*
*              argv         parameters
*
*
* Returns    : == 0         Succeed.
*              <  0         Failed.
*******************************************************************************
*/
void parseConfigFile()
{
    QString cfg_file = QString(QCoreApplication::applicationDirPath() +"/adt_aec.conf");
    QString src_type,dst_type;
    QString version;
    int bypass_mode;
    int dump;

    QSettings configRead(cfg_file, QSettings::IniFormat);
    src_type = configRead.value("/setting/srcType","alsa").toString();
    dst_type = configRead.value("/setting/dstType","alsa").toString();
    bypass_mode = configRead.value("/setting/bypassMode","0").toInt();
    dump = configRead.value("/setting/ecDump","0").toInt();

    version = QString("%1.%2.%3").arg(VER_MAJOR).arg(VER_MINOR).arg(VER_PATCH);
    configRead.setValue("/setting/version", version);

    if(configRead.value("/GPIO/gpio_enable").toString() == "YES") {
        g_AECState.gpio_enable = true;
        g_AECState.gpio_number = configRead.value("/GPIO/gpio_number").toInt();
        //g_AECState.is_enable = false;
        qDebug() << "gpio_enable = YES, gpio_number = " << g_AECState.gpio_number;
    }
    else {
        g_AECState.gpio_enable = false;
        g_AECState.gpio_number = -1;
        //g_AECState.is_enable = true;
        qDebug() << "gpio_enable = NO";
    }

    //qDebug()<<"src"<<src_type<<"dst"<<dst_type;
    qDebug() << "version = " << version
             << "\nbypass = " << bypass_mode
             << "\ndump = " << dump ;

    g_AECState.is_ec_dump = dump;
    g_AECState.by_pass_mode = bypass_mode;
    g_AECState.pcm_src  = PCMALSA;
    g_AECState.pcm_dest = PCMALSA;
}

/*
*******************************************************************************
*                             initAECEngine
*
* Description: This function is the entry of program.
*
* Arguments  : argc         count of parameters
*
*              argv         parameters
*
*
* Returns    : == 0         Succeed.
*              <  0         Failed.
*******************************************************************************
*/
static int initAECEngine()
{
    g_AECState.hAEC = NULL;
    g_AECState.cleanfile = NULL;
    g_AECState.echofile = NULL;
    g_AECState.reffile = NULL;
    g_AECState.stereofile = NULL;

    IAECG4_Params MyParams = {
        // Base Parameters
        sizeof(IAECG4_Params),
        0,          //LockCallback_t
        FRAME_SIZE, //FRAME_SIZE // modified by luhuadong at 20170323
        0,          // AntiHowlEnable
        8000,       //SAMPLING_RATE
        4000,       //SAMPLING_RATE/2
        0,          //FixedBulkDelayMSec // modified by luhuadong at 20170323
        0,          //TailSearchSamples
        0,          //InitialBulkDelay
        128,        // ADT_Int16 ActiveTailLengthMSec
        128,        //32,       //ADT_Int16 TotalTailLengthMSec
        1,          //0,//10,   //ADT_Int16 txNLPAggressiveness
        20,         //0,//33,   //ADT_Int16 MaxTxLossSTdB;
        15,         //0, //12, //12, //ADT_Int16 MaxTxLossDTdB;
        4,          // 12, //ADT_Int16 MaxRxLossdB;
        0,          //InitialRxOutAttendB
        -85,        // ADT_Int16 TargetResidualLeveldBm;
        -90,        //-60,    // ADT_Int16 MaxRxNoiseLeveldBm;
        -25,        // ADT_Int16 worstExpectedERLdB
        -3,         //-3, //6, //      //RxSaturateLeveldBm
        1,          // ADT_Int16 NoiseReduction1Setting
        0,          // ADT_Int16 NoiseReduction2Setting
        1,          //CNGEnable
        0,          //0,      //fixedGaindB10

        // TxAGC Parameters
        0,          // ADT_Int8 AGCEnable;
        10,         // ADT_Int8 AGCMaxGaindB;
        10,         //ADT_Int8 AGCMaxLossdB;
        -15,        // ADT_Int8 AGCTargetLeveldBm;
        -40,        //ADT_Int8 AGCLowSigThreshdBm;

        // RxAGC Parameters
        0,          // ADT_Int8 AGCEnable;
        10,         // ADT_Int8 AGCMaxGaindB;
        15,         //ADT_Int8 AGCMaxLossdB;
        -10,        // ADT_Int8 AGCTargetLeveldBm;
        -40,        //ADT_Int8 AGCLowSigThreshdBm;
        0,          //RxBypass
        0,          //ADT_Int16 maxTrainingTimeMSec,
        -40,        //trainingRxNoiseLeveldBm
        0,          //ADT_Int16 pTxEqualizer
        0,          //mipsMemReductionSetting
        0,          //mipsReductionSetting2
        0           //reserved
    };

    if ((g_AECState.hAEC = AECG4_ADT_create(0, &MyParams)) == 0) {
        g_AECState.by_pass_mode = 1;
        qDebug("AECG4_ADT_Create(): error, entering bypass mode");
        return -1;
    }

    //g_AECState.is_enable = true;

#ifdef EC_DUMP
    if(g_AECState.is_ec_dump)
    {
        g_AECState.echofile  = fopen("/opt/aec/pcmecho.raw", "w");
        g_AECState.reffile   = fopen("/opt/aec/pcmref.raw", "w");
        g_AECState.cleanfile = fopen("/opt/aec/pcmclean.raw", "w");
        g_AECState.stereofile= fopen("/opt/aec/pcmstereo.raw", "w");
    }
#endif
    return 0;
}

/*
*******************************************************************************
*                              initALSAPlayback
*
* Description: This function is the entry of program.
*
* Arguments  : argc         count of parameters
*
*              argv         parameters
*
*
* Returns    : == 0         Succeed.
*              <  0         Failed.
*******************************************************************************
*/
bool initALSAPlayback(bool outputOnly, char *playbackDev)
{
    snd_pcm_t *pcm_handle;
    snd_pcm_hw_params_t *hwparams;
    snd_pcm_sw_params_t *swparams;
    char *pcm_name;
    pcm_name = strdup("plughw:0,0");
    //int rate = SAMPLE_RATE; /* Sample rate */
    int exact_rate;
    int period_size=FRAME_SIZE;       /* Number of periods */
    //int buffer_size=period_size *2*2*2;
    int buffer_size=period_size * 2; // modified by luhuadong at 20170323
    int error;
    int mode;

    // modified by luhuadong at 20170323
    qDebug(">>>>>> initALSAPlayback  buffer_size = %d\n", buffer_size);

    /* Allocate the snd_pcm_hw_params_t structure on the stack. */
    snd_pcm_hw_params_alloca(&hwparams);

    /* Open PCM. The last parameter of this function is the mode. */
    /* If this is set to 0, the standard mode is used. Possible   */
    /* other values are SND_PCM_NONBLOCK and SND_PCM_ASYNC.       */
    /* If SND_PCM_NONBLOCK is used, read / write access to the    */
    /* PCM device will return immediately. If SND_PCM_ASYNC is    */
    /* specified, SIGIO will be emitted whenever a period has     */
    /* been completely processed by the soundcard.                */

    if(outputOnly)
        mode=0;
    else
        mode=SND_PCM_NONBLOCK;

    if (snd_pcm_open(&pcm_handle, playbackDev, SND_PCM_STREAM_PLAYBACK, mode) < 0)
    {
        qDebug("Error opening PCM device %s\n", playbackDev);
        return(false);
    }

    /* Init hwparams with full configuration space */
    if (snd_pcm_hw_params_any(pcm_handle, hwparams) < 0)
    {
        qDebug("Can not configure this PCM device.\n");
        return(false);
    }


    //fprintf(stderr,"aft InitALSAPlayback 4 \n");
    /* Set access type. This can be either    */
    /* SND_PCM_ACCESS_RW_INTERLEAVED or       */
    /* SND_PCM_ACCESS_RW_NONINTERLEAVED.      */
    /* There are also access types for MMAPed */
    /* access, but this is beyond the scope   */
    /* of this introduction.                  */
    if (snd_pcm_hw_params_set_access(pcm_handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
        qDebug("Error setting access.\n");
        return(false);
    }

    /* Set sample format */
    if (snd_pcm_hw_params_set_format(pcm_handle, hwparams, SND_PCM_FORMAT_S16_LE) < 0) {
        qDebug("Error setting format.\n");
        return(false);
    }

    /* Set sample rate. If the exact rate is not supported */
    /* by the hardware, use nearest possible rate.         */
    exact_rate = SAMPLE_RATE;
    if (snd_pcm_hw_params_set_rate_near(pcm_handle, hwparams, &exact_rate, 0) < 0) {
        qDebug("Error setting rate.\n");
        return(false);
    }
    if (SAMPLE_RATE != exact_rate) {
        qDebug("The rate %d Hz is not supported by your hardware.\n ==> Using %d Hz instead.\n", (int)SAMPLE_RATE, exact_rate);
    }

    /* Set number of channels */
    if (snd_pcm_hw_params_set_channels(pcm_handle, hwparams, 2) < 0) {
        qDebug("Error setting channels.\n");
        return(false);
    }

    /* Set period size. */
    if (snd_pcm_hw_params_set_period_size_near(pcm_handle, hwparams, (snd_pcm_uframes_t*)&period_size, 0) < 0) {
        qDebug("Error setting periods.\n");
        return(false);
    }
    qDebug("Period size set to %d\n", period_size);


    if (error=snd_pcm_hw_params_set_buffer_size_near(pcm_handle, hwparams,  (snd_pcm_uframes_t*)&buffer_size) < 0)
    {
        qDebug("Error setting buffersize. %s\n", snd_strerror (error));
        return(false);
    }
    qDebug("playback Buffer size set to %d\n", buffer_size);

    /* Apply HW parameter settings to */
    /* PCM device and prepare device  */
    if (snd_pcm_hw_params(pcm_handle, hwparams) < 0)
    {
        qDebug("Error setting HW params.\n");
        return(false);
    }

    g_AECState.audioInfo.pcm_handle_playback = pcm_handle;


    snd_pcm_uframes_t val;
    snd_pcm_sw_params_malloc(&swparams);
    snd_pcm_sw_params_current(pcm_handle, swparams);
    snd_pcm_sw_params_get_start_threshold(swparams, &val);

    return true;
}

/*
*******************************************************************************
*                             initALSAPlayback
*
* Description: This function is the entry of program.
*
* Arguments  : argc         count of parameters
*
*              argv         parameters
*
*
* Returns    : == 0         Succeed.
*              <  0         Failed.
*******************************************************************************
*/
bool initALSACapture(char *captureDev)
{

    int exact_rate;
    int period_size= FRAME_SIZE;
    //int buffer_size=period_size *2*2*2;
    int buffer_size=period_size * 2; // modified by luhuadong at 20170323
    int error;
    char *pcm_name;
    snd_pcm_t *pcm_handle;
    snd_pcm_hw_params_t *hwparams;
    snd_pcm_sw_params_t *swparams;
    pcm_name = strdup("plughw:0,0");

    // add by luhuadong at 20170323
    qDebug(">>>>>> initALSACapture  buffer_size = %d\n", buffer_size);

    /* Allocate the snd_pcm_hw_params_t structure on the stack. */
    snd_pcm_hw_params_alloca(&hwparams);


    /* Open PCM. The last parameter of this function is the mode. */
    /* If this is set to 0, the standard mode is used. Possible   */
    /* other values are SND_PCM_NONBLOCK and SND_PCM_ASYNC.       */
    /* If SND_PCM_NONBLOCK is used, read / write access to the    */
    /* PCM device will return immediately. If SND_PCM_ASYNC is    */
    /* specified, SIGIO will be emitted whenever a period has     */
    /* been completely processed by the soundcard.                */
    if (snd_pcm_open(&pcm_handle, captureDev, SND_PCM_STREAM_CAPTURE, 0) < 0)
    {
        qDebug("Error opening PCM device %s\n", captureDev);
        return(false);
    }

    /* Init hwparams with full configuration space */
    if (snd_pcm_hw_params_any(pcm_handle, hwparams) < 0)
    {
        qDebug("Can not configure this PCM device.\n");
        return(false);
    }

    /* Set access type. */
    if (snd_pcm_hw_params_set_access(pcm_handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
        qDebug("Error setting access.\n");
        return(false);
    }

    /* Set sample format */
    if (snd_pcm_hw_params_set_format(pcm_handle, hwparams, SND_PCM_FORMAT_S16_LE) < 0) {
        qDebug("Error setting format.\n");
        return(false);
    }

    /* Set sample rate. If the exact rate is not supported */
    /* by the hardware, use nearest possible rate.         */
    exact_rate = SAMPLE_RATE;
    if (snd_pcm_hw_params_set_rate_near(pcm_handle, hwparams, &exact_rate, 0) < 0) {
        qDebug("Error setting rate.\n");
        return(false);
    }
    if (SAMPLE_RATE != exact_rate) {
        qDebug("The rate %d Hz is not supported by your hardware.\n ==> Using %d Hz instead.\n", (int)SAMPLE_RATE, exact_rate);
    }

    /* Set number of channels */
    if (snd_pcm_hw_params_set_channels(pcm_handle, hwparams, 2) < 0) {
        qDebug("Error setting channels.\n");
        return(false);
    }

    /* Set period size. */
    if (snd_pcm_hw_params_set_period_size_near(pcm_handle, hwparams, (snd_pcm_uframes_t*)&period_size, 0) < 0) {
        qDebug("Error setting periods.\n");
        return(false);
    }
    qDebug("Period size set to %d\n", period_size);

    /* Set buffer size (in frames). The resulting latency is given by */
    /* latency = periodsize * periods / (rate * bytes_per_frame)     */
    if (error=snd_pcm_hw_params_set_buffer_size_near(pcm_handle, hwparams,  (snd_pcm_uframes_t*)&buffer_size) < 0)
    {
        qDebug("Error setting buffersize. %s\n", snd_strerror (error));
        return(false);
    }
    qDebug("capture Buffer size set to %d\n", buffer_size);

    /* Apply HW parameter settings to */
    /* PCM device and prepare device  */
    if (snd_pcm_hw_params(pcm_handle, hwparams) < 0)
    {
        qDebug("Error setting HW params.\n");
        return(false);
    }

    g_AECState.audioInfo.pcm_handle_capture = pcm_handle;

    snd_pcm_sw_params_malloc(&swparams);
    snd_pcm_sw_params_current(pcm_handle, swparams);
    snd_pcm_uframes_t val;
    snd_pcm_sw_params_get_start_threshold(swparams, &val);

    return true;
}

/*
*******************************************************************************
*                                initAudio
*
* Description: This function is the entry of program.
*
* Arguments  : outputOnly
*
*              captureDev
*
*              playbackDev
*
*
* Returns    : true          Succeed.
*              false         Failed.
*******************************************************************************
*/
bool initAudio(bool outputOnly, char *captureDev, char *playbackDev)
{
    system("amixer -q set 'Capture Mux' LINE_IN &");
    qDebug()<<"Audio 'Capture Mux' set to LINE_IN.";

#ifdef PLAYBACK
    //init playback
    if(!initALSAPlayback(outputOnly, playbackDev))
    {
        qDebug()<<"Audio playback init failed";
        return false;
    }
#endif

    if(!outputOnly)
    {
        //init capture
        if(!initALSACapture(captureDev))
        {
            qDebug()<< "Audio capture init failed";
            return false;
        }
    }

    return true;
}

//recovery callback in case of error
/*
*******************************************************************************
*                               xrunRecovery
*
* Description: This function is the entry of program.
*
* Arguments  : argc         count of parameters
*
*              argv         parameters
*
*
* Returns    : == 0         Succeed.
*              <  0         Failed.
*******************************************************************************
*/
static int xrunRecovery(snd_pcm_t *handle, int error)
{
    switch(error)
    {
    case -EPIPE:    // Buffer Over-run
        if (error = snd_pcm_prepare(handle)< 0)
            qDebug(" Buffer overrrun/underrun cannot be recovered, snd_pcm_prepare fail: %s\n", snd_strerror(error));
        return 0;
        break;

    case -ESTRPIPE: //suspend event occurred
        //EAGAIN means that the request cannot be processed immediately
        while ((error = snd_pcm_resume(handle)) == -EAGAIN)
            sleep(1);// wait until the suspend flag is clear

        if (error < 0) // error case
        {
            if (error = snd_pcm_prepare(handle) < 0)
                qDebug("Suspend cannot be recovered, snd_pcm_prepare fail: %s\n", snd_strerror(error));
        }
        return 0;
        break;

    case -EBADFD: //Error PCM descriptor is wrong
        break;

    default:
        break;
    }
    return error;
}

/*
*******************************************************************************
*                             shutdownAECEngine
*
* Description: This function is the entry of program.
*
* Arguments  : argc         count of parameters
*
*              argv         parameters
*
*
* Returns    : == 0         Succeed.
*              <  0         Failed.
*******************************************************************************
*/
static bool shutdownAECEngine()
{
    if (g_AECState.hAEC) {
        AECG4_ADT_delete(g_AECState.hAEC);
        g_AECState.hAEC= NULL;
    }

#ifdef EC_DUMP
    if(g_AECState.is_ec_dump)
    {
        if (g_AECState.echofile)
            fclose(g_AECState.echofile);
        if (g_AECState.reffile)
            fclose(g_AECState.reffile);
        if (g_AECState.cleanfile)
            fclose(g_AECState.cleanfile);
        if (g_AECState.stereofile)
            fclose(g_AECState.stereofile);
    }
#endif
    return true;
}

/*
*******************************************************************************
*                              shutdownAudio
*
* Description: This function is the entry of program.
*
* Arguments  : argc         count of parameters
*
*              argv         parameters
*
*
* Returns    : == 0         Succeed.
*              <  0         Failed.
*******************************************************************************
*/
static bool shutdownAudio()
{
    if(g_AECState.audioInfo.pcm_handle_playback)
        snd_pcm_close(g_AECState.audioInfo.pcm_handle_playback);
    if(g_AECState.audioInfo.pcm_handle_capture)
        snd_pcm_close(g_AECState.audioInfo.pcm_handle_capture);

    return true;
}

/*
*******************************************************************************
*                              TimevalSubtract
*
* Description: This function is the entry of program.
*
* Arguments  : argc         count of parameters
*
*              argv         parameters
*
*
* Returns    : == 0         Succeed.
*              <  0         Failed.
*******************************************************************************
*/
static int TimevalSubtract(struct timeval *result,struct timeval tv_begin,struct timeval tv_end)
{

    /* Perform the carry for the later subtraction by updating y. */
    if (tv_end.tv_usec < tv_begin.tv_usec)
    {
        int nsec = (tv_begin.tv_usec - tv_end.tv_usec) / 1000000 + 1;
        tv_begin.tv_usec -= 1000000 * nsec;
        tv_begin.tv_sec += nsec;
    }
    if (tv_end.tv_usec - tv_begin.tv_usec > 1000000)
    {
        int nsec = (tv_end.tv_usec - tv_begin.tv_usec) / 1000000;
        tv_begin.tv_usec += 1000000 * nsec;
        tv_begin.tv_sec -= nsec;
    }

    /* Compute the time remaining to wait.
    tv_usec is certainly positive. */
    result->tv_sec = tv_end.tv_sec - tv_begin.tv_sec;
    result->tv_usec = tv_end.tv_usec - tv_begin.tv_usec;

    /* Return 1 if result is negative. */
    return tv_end.tv_sec < tv_begin.tv_sec;
}

///////////////////////////////////
//
//	BackgroundThread() runs every 20ms
//
///////////////////////////////////
/*
*******************************************************************************
*                              backGroundThread
*
* Description: This function is the entry of program.
*
* Arguments  : argc         count of parameters
*
*              argv         parameters
*
*
* Returns    : == 0         Succeed.
*              <  0         Failed.
*******************************************************************************
*/
void backGroundThread(void *ptr)
{
    struct timeval tv_begin, tv_end, tv_result;
    int usToSleep;
    int countDown = 50;

    pthread_setschedparam(pthread_self(),SCHED_OTHER, NULL);

    while(1)
    {
        gpioHandle();

        gettimeofday(&tv_begin, NULL);

        if(--countDown == 0)
        {
            //qDebug("OneSecTick");
            countDown = 50;

#ifdef PRINT_ERL
            IAECG4_Status Status;
            AECG4_ADT_control(g_AECState.hAEC,IAECG4_GETSTATUS,(IAECG4_Status *) &Status);
            qDebug()<<"erldB10BestEstimate "<<Status.erldB10BestEstimate;
            qDebug()<<"worstPerBinERLdB10BestEstimate "<<Status.worstPerBinERLdB10BestEstimate;
#endif
        }

        gettimeofday(&tv_end, NULL);
        if(TimevalSubtract(&tv_result,tv_begin, tv_end)<0)
            continue; //result is negative

        usToSleep = 20000 - tv_result.tv_usec;

        if(usToSleep < 0)
            continue;
        usleep(usToSleep);
    }

    return;
}

/*
*******************************************************************************
*                                  MAIN
*
* Description: This function is the entry of program.
*
* Arguments  : argc         count of parameters
*
*              argv         parameters
*
*
* Returns    : == 0         Succeed.
*              <  0         Failed.
*******************************************************************************
*/
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // prints out debug messages, warnings, critical and fatal error messages
    qInstallMsgHandler(customMessageHandler);

    // Initialization
    init();

    return a.exec();
}

/*
*******************************************************************************
*                                  init
*
* Description: This function is the entry of program.
*
* Arguments  : argc         count of parameters
*
*              argv         parameters
*
*
* Returns    : == 0         Succeed.
*              <  0         Failed.
*******************************************************************************
*/
void init()
{
    int ret;
    pthread_t pThreadId;
    pthread_t bThreadId;
    char *play_back_dev = strdup("plughw:0,0");
    char *capture_dev   = strdup("plughw:0,0");

    qDebug()<<"/*******************************************/";

    parseConfigFile();

    if(g_AECState.gpio_enable) {
        ret = gpioInit();
        if (ret<0) {
            qDebug("GPIO initialization failed.!");
        }
    }

    if(!initAudio(0,capture_dev,play_back_dev))
    {
        qDebug("Audio initialization failed .Exit!  ");
        return;
    }

    ret = initAECEngine();
    if(ret < 0)
    {
        return;
    }

    system("amixer -q set 'PCM' 192 &");
    qDebug()<<"Audio 'PCM' set to 192.";

    if(g_AECState.gpio_enable) {
        ret = pthread_create(&bThreadId, NULL,(void *) &backGroundThread, NULL);
        if(ret != 0)
        {
            qDebug("backGroundThread thread create failed .Exit!  ");
            return;
        }
        else {
            qDebug("backGroundThread created.");
        }
    }

    ret = pthread_create(&pThreadId, NULL,(void *) &frameProcess, NULL);
    if(ret != 0)
    {
        qDebug("frameProcess thread create failed .Exit!  ");
        return;
    }
    else {
        qDebug("frameProcessThread created.");
    }

    qDebug("adt_aec init OK.");
}


/*	inputs[0]= reference signal from far end (sent to soundcard)
 *	inputs[1]= near speech & echo signal (read from soundcard)
 *	outputs[0]=  is a copy of inputs[0] to be sent to soundcard
 *	outputs[1]=  near end speech, echo removed - towards far end
*/
/*
*******************************************************************************
*                               frameProcess
*
* Description: This function is the entry of program.
*
* Arguments  : argc         count of parameters
*
*              argv         parameters
*
*
* Returns    : == 0         Succeed.
*              <  0         Failed.
*******************************************************************************
*/
void frameProcess(void *ptr )
{
    bool restartOutput=false;
    short int RxOut[FRAME_SIZE];  //not use it indeed
    int readerror, writeerror;
    int i,retVal;
    int j = 0;

    ADT_Int16 echo[MAX_FRAME_SIZE];
    ADT_Int16 ref[MAX_FRAME_SIZE];
    ADT_Int16 clean[MAX_FRAME_SIZE];

    ADT_Int16 stereo[MAX_FRAME_SIZE*2];
    ADT_Int16 out_stereo[MAX_FRAME_SIZE*2];
    snd_pcm_uframes_t size = FRAME_SIZE;

    qDebug("frameProcess begin");

#ifdef PLAYBACK
    retVal = snd_pcm_link(g_AECState.audioInfo.pcm_handle_capture, g_AECState.audioInfo.pcm_handle_playback);
    //prefill output buffer to prevent xflows
    memset(out_stereo, 0, sizeof(out_stereo));
    retVal = snd_pcm_writei(g_AECState.audioInfo.pcm_handle_playback, out_stereo, size);
    retVal = snd_pcm_writei(g_AECState.audioInfo.pcm_handle_playback, out_stereo, size);
#endif
    while(1)
    {
        //qDebug("frameProcess begin");
        while(g_AECState.is_enable)
        //while(1)
        {
            readerror = snd_pcm_readi(g_AECState.audioInfo.pcm_handle_capture, stereo, size);
            if (readerror < 0)
            {
                qDebug("alsa read error (%s)\n", snd_strerror (readerror));
                if (xrunRecovery(g_AECState.audioInfo.pcm_handle_capture, readerror) < 0) {
                    qDebug("microphone: Write error: %s\n", snd_strerror(readerror));
                }
                memset(out_stereo, 0, sizeof(out_stereo));
                continue;
            }
            else
            {
                if(readerror != size)
                    qDebug("Short on samples captured: %d\n", readerror);

#ifdef EC_DUMP
                if(g_AECState.is_ec_dump)
                {
                    //fwrite(stereo, sizeof(ADT_Int16), size*2, g_AECState.stereofile); // 2==chanels
                }
#endif
                for(i=0,j=0;i<size*2;i=i+2)
                {

                    //fwrite(stereo+i, sizeof(ADT_Int16), 1, g_AECState.echofile);       //left
                    //fwrite(stereo+1+i, sizeof(ADT_Int16), 1, g_AECState.reffile);       //right

                    memcpy(ref+j,stereo+i,sizeof(ADT_Int16));
                    memcpy(echo+j,stereo+1+i,sizeof(ADT_Int16));
                    j++;
                }
#ifdef EC_DUMP
                if(g_AECState.is_ec_dump)
                {
                    fwrite(echo, sizeof(ADT_Int16), size, g_AECState.echofile);
                    fwrite(ref, sizeof(ADT_Int16), size, g_AECState.reffile);
                }
#endif
                if(g_AECState.is_enable == false)
                    break;
#ifdef PLAYBACK
                if(restartOutput)
                {
                    //restarting, write some silence data to output to prefil buffer and prevent underrun
                    qDebug("Restarting output\n");
                    snd_pcm_prepare(g_AECState.audioInfo.pcm_handle_playback);
                    memset(out_stereo, 0, sizeof(out_stereo));
                    for(i=0;i<1;i++)
                    {
                        snd_pcm_writei(g_AECState.audioInfo.pcm_handle_playback, out_stereo, size);
                        snd_pcm_writei(g_AECState.audioInfo.pcm_handle_playback, out_stereo, size);
                        snd_pcm_writei(g_AECState.audioInfo.pcm_handle_playback, out_stereo, size);
                    }
                    restartOutput=false;
                }
#endif

                if(g_AECState.by_pass_mode)
                {
#ifdef PLAYBACK
                    for(i=0,j=0;j<size;i=i+2)
                    {
                        memcpy(out_stereo+i,echo+j,sizeof(ADT_Int16));
                        memset(out_stereo+1+i, 0, sizeof(ADT_Int16));
                        j++;
                    }

                    while ((writeerror = snd_pcm_writei (g_AECState.audioInfo.pcm_handle_playback, out_stereo, size)) < 0)
                    {
                        if (writeerror == -EAGAIN)
                            continue;
                        qDebug("alsa write error (%s)\n", snd_strerror (writeerror));
                        xrunRecovery(g_AECState.audioInfo.pcm_handle_playback, writeerror);
                        restartOutput=true;
                        break;
                    }
                    if(writeerror >=0 &&(writeerror!= size))
                        qDebug("Short on samples played: %d\n", writeerror);
#endif
                }
                else
                {
                    AECG4_ADT_apply(g_AECState.hAEC, (short int *)ref, RxOut, (short int *)echo, (short int *)clean);

#ifdef EC_DUMP
                    if(g_AECState.is_ec_dump)
                    {
                        fwrite(clean, sizeof(ADT_Int16), size, g_AECState.cleanfile);
                    }
#endif

#ifdef PLAYBACK
                    for(i=0,j=0;j<size;i=i+2)
                    {
                        memcpy(out_stereo+i,clean+j,sizeof(ADT_Int16));
                        memset(out_stereo+1+i, 0, sizeof(ADT_Int16));
                        j++;
                    }

                    while ((writeerror = snd_pcm_writei (g_AECState.audioInfo.pcm_handle_playback, out_stereo, size)) < 0)
                    {
                        if (writeerror == -EAGAIN)
                            continue;
                        qDebug("alsa write error (%s)\n", snd_strerror (writeerror));
                        xrunRecovery(g_AECState.audioInfo.pcm_handle_playback, writeerror);
                        restartOutput=true;
                        break;
                    }
                    if(writeerror >=0 &&(writeerror!= size))
                        qDebug("Short on samples played: %d\n", writeerror);
#endif
                }
            }
        } // while(g_AECState.is_enable)

        usleep(2*1000);

    } // while(1)

    //deinit audio
    qDebug("Shutting down Audio");
    shutdownAudio();
    //shutdown engine
    qDebug("Shutting down ADT AEC Engine");
    shutdownAECEngine();
    return;
}

/*
*******************************************************************************
*                             getInputFrame
*
* Description: This function is the entry of program.
*
* Arguments  : argc         count of parameters
*
*              argv         parameters
*
*
* Returns    : == 0         Succeed.
*              <  0         Failed.
*******************************************************************************
*/
static void getInputFrame(ADT_Int16 *ref, ADT_Int16 *echo)
{    

}

/*
*******************************************************************************
*                              outPutFrame
*
* Description: This function is the entry of program.
*
* Arguments  : argc         count of parameters
*
*              argv         parameters
*
*
* Returns    : == 0         Succeed.
*              <  0         Failed.
*******************************************************************************
*/
static void outPutFrame(ADT_Int16 *clean)
{

}
