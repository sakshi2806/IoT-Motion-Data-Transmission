const char* ssid = "ssid";
const char* password =  "password";

const char* serverURL = "http://your-server.com/data.php"; // Replace with your server URL

HTTPClient http;
WiFiClient client;

#define led 2
String data="";


uint32_t timer1,timer2,timer3,timer4;
//Data Points related variables
const int N = 1024;
const int N_set_gather = 4;
const int N_set_send = 1;
const int N_gather_size = N / N_set_gather; 
const int N_send_size = N_gather_size / N_set_send; 
int gathering_status[N_set_gather] ;  
int sending_status[N_set_gather];  

//Mac id related variables
uint64_t chipid;
char id_[6];
char id_1[10];
char kid[12];
char k_id[12];
int device_id;


/////////////MPU Setup/////////////
//VSPI
#define MPU_CS_PIN        5

int16_t Ax[N], Ay[N], Az[N], Gx[N], Gy[N], Gz[N];

float Acc_bias[3];
float Gyro_bias[3];


bool useSPI = true;    
const int16_t NUM_OF_CALIBRATION_SAMPLES = 5000;
unsigned long lastMicros = 0;
const int MAX_SAMPLING_FREQ = 2000; 
unsigned long MINIMUM_SAMPLING_DELAY_uSec = (unsigned long)(1 * 1000000 / MAX_SAMPLING_FREQ);
const int16_t gyr_factor = 100;
const int16_t acc_factor = 10000;

String control_val = "";


///////////////CONSTRUCTORS//////////////
MPU9250_WE myMPU9250 = MPU9250_WE(&SPI, MPU_CS_PIN, useSPI);  

///////////////FUNCTION-PROTO////////////
void calibrate_MPU(float acc_bias[], float gyro_bias[]);
void connect_to_wifi(const char* ssid,const char* password);

//////////////FREE-RTOS SETUP///////////////
TaskHandle_t MPU_GET_h = NULL, HTTP_SEND_h = NULL;

bool send_Flag=false;
