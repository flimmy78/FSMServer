#ifndef FSM_STATUSSTRUCT
#define FSM_STATUSSTRUCT
#define srow_cnt 8
#define scolumn_cnt 12
#define status_cnt srow_cnt*scolumn_cnt
struct fsm_status_element
{
int row;
int column;
char state[32];
char fsmdevcode[32];
short devid;
};
struct fsm_device_config
{
    int row;
    int column;
    unsigned short IDDevice;
    unsigned short Len;
    char config[1000];
};
struct FSMSSetconfig
{
    unsigned char cmd; 
    unsigned short IDDevice;
    struct fsm_device_config config;
};
enum FSMSSetconfigCmd
{
    SetFSMSetting=1,
    GetFSMSetting=2
};
struct FSMSSetconfigParam
{
    unsigned char cmd; 
    unsigned short IDDevice;
   struct fsm_device_config*  config;
};

struct fsm_devices_config
{
    struct fsm_device_config setel[srow_cnt][scolumn_cnt];
};
struct fsm_statusstruct
{
struct fsm_status_element statel[srow_cnt][scolumn_cnt];
};
struct fsm_ClientSetting_Setting
{
    unsigned short id;
    unsigned short size_row;
    unsigned short size_column;
};
struct fsm_ServerSetting_Setting
{
    unsigned short size_row;
    unsigned short size_column;
};
struct fsm_Setting_Setting
{
  struct fsm_ClientSetting_Setting fsmcs;
  struct fsm_ServerSetting_Setting fsmss;  
};


enum FSMST_VidDevice
{
    FSMDeviceConfig=1, ///<  Модуль Конфигурации
    FSMDeviceStatistic=2, ///<  Модуль Конфигурации
};
/*!
\brief ПодВид устроства
*/
enum FSMST_PodVidDevice
{
   ComputerStatistic=1 ///< ComputerStatistic
};
/*!
\brief Род устроства
*/
enum FSMST_RodDevice
{
    PCx86=1,///< PCx86
};
enum FSMST_Cmd /*0*****125*/
{
    GetStatistic=1,
    AnsGetStatistic=2
};
enum FSMS_Cmd /*0*****125*/
{
    GetSet=1,
    AnsGetSet=2,
    SetSetting=3,
    AnsSetSetting=4,
    SetSettingClient=5,
    AnsSetSettingClient=6,
    SendSettingFull=7
};

enum FSMCDPC_VidDevice
{
    Computer=1, ///<  Модуль Конфигурации
    Device=2,
};
/*!
\brief ПодВид устроства
*/
enum FSMCDPC_PodVidDevice
{
   PC=1, ///< ComputerStatistic
   GPIO=2
};
/*!
\brief Род устроства
*/
enum FSMCDPC_RodDevice
{
    ARM=1,///< PCx86
    Bit_8=2
};

enum FSMIOCTLStat_Cmd
{
    FSMIOCTLStat_Read,
    FSMIOCTLStat_Requst
};
enum FSM_eventlist_status
{
    
    /**Statistick Event List 0x40 - 0x6F **/
    FSM_ControlDeviceRun=0x40,
    FSM_StaticServerRun=0x41,
    FSM_SettingServerRun=0x42,
    
    
};
enum FSM_Property_time
{
    FSMP_INT,
    FSMP_STRING,
};

typedef void (*UpdateDataProperty)(struct FSM_PropertyDevice*);

struct FSM_PropertyDevice
{
    char fsmdevcode[32];
    unsigned short devid;
    char PropertyCode[32];
    void * Property;
    unsigned short pr_size;
    UpdateDataProperty udp;
    
};
//  struct FSM_PropertyDevice pdl[FSM_PropertyTreeSize];
    
enum FSM_GPIO_Bit_Enum
{
    FSM_GPIO_Bit_0=1,
    FSM_GPIO_Bit_1=2,
    FSM_GPIO_Bit_2=4,
    FSM_GPIO_Bit_3=8,
    FSM_GPIO_Bit_4=16,
    FSM_GPIO_Bit_5=32,
    FSM_GPIO_Bit_6=64,
    FSM_GPIO_Bit_7=128,
};

enum FSM_GPIO_Bit_Cmd
{
    FSM_ON_Bit,
    FSM_Eror_ON_Bit,
    FSM_OFF_Bit,
    FSM_Eror_OFF_Bit,
    FSM_Reset_Bit,
    FSM_Event_Bit,
};

void FSM_GPIO_SetBit(enum FSM_GPIO_Bit_Enum Pin);

void FSM_GPIO_ReSetBit(enum FSM_GPIO_Bit_Enum Pin);

void FSM_GPIO_Set_Input(enum FSM_GPIO_Bit_Enum Pin);

void FSM_GPIO_Set_Output(enum FSM_GPIO_Bit_Enum Pin);

unsigned char FSM_GPIO_Get_Status(enum FSM_GPIO_Bit_Enum Pin);

void FSM_GPIO_Reset_timer_callback( unsigned long data );

void FSM_GPIO_Impulse_timer_callback( unsigned long data );

void FSM_GPIO_Reset(void);

void FSM_GPIO_EventEror(void);

void FSM_GPIO_Ctl_Reset(void);
void FSM_GPIO_Ctl_Eror(void);
void FSM_GPIO_Ctl_SetBit(enum FSM_GPIO_Bit_Enum Pin);
void FSM_GPIO_Ctl_ReSetBit(enum FSM_GPIO_Bit_Enum Pin);
void FSM_GPIO_Ctl_Error_ON(void);
void FSM_GPIO_Ctl_Error_OFF(void);

#endif // FSM_STATUSSTRUCT
