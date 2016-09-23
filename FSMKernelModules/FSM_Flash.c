#include <linux/init.h>
#include <linux/module.h>
#include "FSM/FSMDevice/FSM_DeviceProcess.h"
#include <linux/fs.h> 

struct FSM_DeviceFunctionTree dft;
struct FSM_DeviceTree* dftv;
struct FSM_SendCmd fsmsc;
struct FSMFlash_Control flctl[FSM_FlasherSize];
int FSM_FlashFirmwareOpen(struct FSM_DeviceTree* to_dt,int n)
{
    struct file *f; 
    int numb;
    char filename[100];
    sprintf(filename,"/fsm/firmware/t%uv%upv%uk%u.fsmflash",
                        to_dt->dt->type,
                        to_dt->dt->VidDevice,
                        to_dt->dt->PodVidDevice,
                        to_dt->dt->KodDevice);
    f = filp_open( filename, O_RDONLY, 0 ); 
    if( IS_ERR( f ) ) { 
        printk( "Firmware not found" ); 
        return -1;
    } 
    numb = kernel_read( f, 0, (char*)&flctl[n].firm, sizeof(struct FSMFirmware) ); 
    filp_close( f, NULL ); 
    if(!(numb)) return -2;
   
    return 0;
}
int FSM_FlashCheckData(int n)
{
    int i=0;
    if(flctl[n].firm.evec.crc32 != flctl[n].firm.svec.crc32) return -1;
    if(flctl[n].firm.evec.size != flctl[n].firm.svec.size) return -2;
    for(i=0;i<128;i++) 
    {
            if(FSM_crc32NT(0,flctl[n].firm.dvec[i].Data,1024)!=flctl[n].firm.dvec[i].crc32)
            {
                return -3-i;
            }
                
    }
    return 0;
}
void FSM_FlashStart(struct FSM_DeviceTree* to_dt)
{     int i=0;
      for(i=0;i<FSM_FlasherSize;i++)
      {
          if((flctl[i].reg==1)&&(flctl[i].dt!=0))  if(flctl[i].dt->IDDevice==to_dt->IDDevice) return;
      }
      for(i=0;i<FSM_FlasherSize;i++)
      {
          if(flctl[i].reg==0) 
          {
              if(FSM_FlashFirmwareOpen(to_dt,i)!=0) return;
              if(FSM_FlashCheckData(i)!=0) return;
              flctl[i].reg=1;
              flctl[i].dt=to_dt;
              flctl[i].state = (char)FSM_Flash_S_Start;
              fsmsc.cmd=FSMFlash_Start;
              fsmsc.IDDevice=to_dt->IDDevice;
              memcpy(fsmsc.Data,(char*)&flctl[i].firm.svec,sizeof(struct FSMFlahData_StartVector));
              to_dt->TrDev->dt->Proc((char*)&fsmsc,FSMH_Header_Size_SendCmd+sizeof(struct FSMFlahData_StartVector),to_dt->TrDev,to_dt);
              break;
          }
      }
     
      
}
EXPORT_SYMBOL(FSM_FlashStart);


void FSM_FlashRecive(char* data, short len, struct FSM_DeviceTree* to_dt)
{
    int i;
    
    switch(((struct FSM_SendCmdTS*)data)->cmd)
    {
        case FSMFlash_Start:
        fsmsc.cmd=FSMFlash_Data;
        fsmsc.IDDevice=to_dt->IDDevice;
        for(i=0;i<FSM_FlasherSize;i++)
        {
          if((flctl[i].reg==1)&&(flctl[i].dt!=0))  if(flctl[i].dt->IDDevice==to_dt->IDDevice)
          {
             memcpy(fsmsc.Data,(char*)&flctl[i].firm.dvec[0],sizeof(struct FSMFlahData_DataVector));
             to_dt->TrDev->dt->Proc((char*)&fsmsc,FSMH_Header_Size_SendCmd+sizeof(struct FSMFlahData_DataVector),to_dt->TrDev,to_dt); 
             flctl[i].state = (char)FSM_Flash_S_Data;
             return;
          }
        }
        
        break;
        case FSMFlash_Execute:
        fsmsc.cmd=FSMFlash_Confirm;
        fsmsc.IDDevice=to_dt->IDDevice;
        for(i=0;i<FSM_FlasherSize;i++)
        {
          if((flctl[i].reg==1)&&(flctl[i].dt!=0))  if(flctl[i].dt->IDDevice==to_dt->IDDevice)
          {
             if(flctl[i].firm.evec.crc32!=((struct FSMFlahData_DataVerifeVector*)(((struct FSM_SendCmdTS*)data)->Data))->crc32)
             {
             fsmsc.cmd=FSMFlash_Confirm;
             to_dt->TrDev->dt->Proc((char*)&fsmsc,FSMH_Header_Size_SendCmd,to_dt->TrDev,to_dt);
             flctl[i].reg=0;
             flctl[i].state = (char)FSM_Flash_S_End;
             }
             else
             {
             fsmsc.cmd=FSMFlash_Execute;
             memcpy(fsmsc.Data,(char*)&flctl[i].firm.evec,sizeof(struct FSMFlahData_EndVector));  
             to_dt->TrDev->dt->Proc((char*)&fsmsc,FSMH_Header_Size_SendCmd+sizeof(struct FSMFlahData_EndVector),to_dt->TrDev,to_dt);
             }
             return;
          }
        }
        break;
        case FSMFlash_Confirm:
        
        break;
        case FSMFlash_Data:
        fsmsc.cmd=FSMFlash_Data;
        fsmsc.IDDevice=to_dt->IDDevice;
        for(i=0;i<FSM_FlasherSize;i++)
        {
          if((flctl[i].reg==1)&&(flctl[i].dt!=0))  if(flctl[i].dt->IDDevice==to_dt->IDDevice)
          {
             if(flctl[i].firm.dvec[((struct FSMFlahData_DataVerifeVector*)(((struct FSM_SendCmdTS*)data)->Data))->num].crc32!=((struct FSMFlahData_DataVerifeVector*)(((struct FSM_SendCmdTS*)data)->Data))->crc32)
             {
             memcpy(fsmsc.Data,(char*)&flctl[i].firm.dvec[((struct FSMFlahData_DataVerifeVector*)(((struct FSM_SendCmdTS*)data)->Data))->num],sizeof(struct FSMFlahData_DataVector));
             }
             else
             {
             if(((struct FSMFlahData_DataVerifeVector*)(((struct FSM_SendCmdTS*)data)->Data))->num == 127)
             {
             fsmsc.cmd=FSMFlash_Execute;
             memcpy(fsmsc.Data,(char*)&flctl[i].firm.evec,sizeof(struct FSMFlahData_EndVector));  
             to_dt->TrDev->dt->Proc((char*)&fsmsc,FSMH_Header_Size_SendCmd+sizeof(struct FSMFlahData_EndVector),to_dt->TrDev,to_dt);
             
             return; 
             }
             else
             {
             memcpy(fsmsc.Data,(char*)&flctl[i].firm.dvec[((struct FSMFlahData_DataVerifeVector*)(((struct FSM_SendCmdTS*)data)->Data))->num+1],sizeof(struct FSMFlahData_DataVector));
             }
             }
             to_dt->TrDev->dt->Proc((char*)&fsmsc,FSMH_Header_Size_SendCmd+sizeof(struct FSMFlahData_DataVector),to_dt->TrDev,to_dt); 
             return;
          }
        }
        break;
    }
}
EXPORT_SYMBOL(FSM_FlashRecive);

void
FSM_FlashDeviceRecive(char* data, short len, struct FSM_DeviceTree* to_dt, struct FSM_DeviceTree* from_dt)
{

    struct FSM_SendCmdTS* fscts = (struct FSM_SendCmdTS*)data;
    //int i;
    // printk( KERN_INFO "FSM SIOCTL,%u \n",fscts->opcode );

    switch(data[0]) {
    case RegDevice:
        FSM_Statstic_SetStatus(to_dt, "ok");
        break;
    case SendCmdToServer: ///< Отправка команды серверу
        break;
    case PacketFromUserSpace: ///< Отправка команды серверу
        switch(fscts->cmd) 
        {
        case FSM_Flash_CTL_Flash:
        dftv = FSM_FindDevice(((unsigned short*)fscts->Data)[0]);
        if(dftv == 0) {
         return;
        }
        FSM_FlashStart(dftv);
        break;
        case FSM_Flash_CTL_GetStatus:
        fscts->Data[0]=flctl[fscts->Data[0]].state;
        break;
        }
        break;

     
    }
}

static int __init FSMFlash_init(void)
{
    struct FSM_DeviceRegistr regp;
    dft.aplayp = 0;
    dft.type = (unsigned char)StatisticandConfig;
    dft.VidDevice = (unsigned char)FSMDeviceConfig;
    dft.PodVidDevice = (unsigned char)FSM_Flash;
    dft.KodDevice = (unsigned char)CTL_FSM_Flash;
    dft.Proc = FSM_FlashDeviceRecive;
    dft.config_len = 0;
    FSM_DeviceClassRegister(dft);

    regp.IDDevice = FSM_TreeSettingID;
    regp.VidDevice = dft.VidDevice;
    regp.PodVidDevice = dft.PodVidDevice;
    regp.KodDevice = dft.KodDevice;
    regp.type = dft.type;
    regp.opcode = RegDevice;
    regp.CRC = 0;
    FSM_DeviceRegister(regp);
    
    fsmsc.opcode=SendCmdGlobalcmdToClient;
    fsmsc.countparam=1;
    fsmsc.CRC=0;
    printk(KERN_INFO "FSM Flash loaded\n");
    return 0;
}
static void __exit FSMFlash_exit(void)
{
    FSM_ClassDeRegister(dft);
    printk(KERN_INFO "FSM Flash module unloaded\n");
}
module_init(FSMFlash_init);
module_exit(FSMFlash_exit);

MODULE_AUTHOR("Gusenkov S.V FSM");
MODULE_DESCRIPTION("FSM Device Manager Module");
MODULE_LICENSE("GPL");