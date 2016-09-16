/*!
\file
\brief Модуль командного управления
\authors Гусенков.С.В
\version 0.0.1_rc1
\date 30.12.2015
*/

#define SUCCESS 0

#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/netdevice.h>

#include "FSM/FSMAudio/FSM_AudioStream.h"
#include <FSM/FSMEthernet/FSMEthernetHeader.h>
#include "FSM/FSMDevice/fcmprotocol.h"
#include "FSM/FSMDevice/fcm_audiodeviceclass.h"
#include "FSM/FSMDevice/FSM_DeviceProcess.h"
#include "FSM/FSMSetting/FSM_settings.h"
#include "FSM/FSMDevice/fsm_statusstruct.h"
#include "FSM/FSM_Client/FSM_client.h"

struct FSM_SendCmdUserspace fsmdat;
struct fsm_ioctl_struct fsmioctl[FSM_IOCTLTreeSize];
static dev_t first;       // Global variable for the first device number
static struct cdev c_dev; // Global variable for the character device structure
static struct class* cl;  // Global variable for the device class

struct fsm_ioctl_struct* FSM_RegisterIOCtl(unsigned int id, IOClientProcess Handler)
{
    int i;
    for(i = 0; i < FSM_IOCTLTreeSize; i++) {
        if(fsmioctl[i].reg == 0) {
            fsmioctl[i].reg = 1;
            fsmioctl[i].id = id;
            fsmioctl[i].Handler = Handler;
            return &fsmioctl[i];
        }
    }
    return 0;
}
EXPORT_SYMBOL(FSM_RegisterIOCtl);

struct fsm_ioctl_struct* FSM_FindIOCtl(unsigned int id)
{
    int i;
    for(i = 0; i < FSM_IOCTLTreeSize; i++) {
        if((fsmioctl[i].reg == 1) && (fsmioctl[i].id == id)) {
            return &fsmioctl[i];
        }
    }
    return 0;
}
EXPORT_SYMBOL(FSM_FindIOCtl);

void FSM_DeleteIOCtl(unsigned int id)
{
    struct fsm_ioctl_struct* evnt = FSM_FindIOCtl(id);
    if(evnt != 0)
        evnt->reg = 0;
}
EXPORT_SYMBOL(FSM_DeleteIOCtl);

static int device_open(struct inode* inode, struct file* file)
{
    // printk( KERN_INFO "FSM SDEvOpen\n" );
    return SUCCESS;
}

static int device_release(struct inode* inode, struct file* file)
{
    // printk( KERN_INFO "FSM SDEvClose\n" );
    return SUCCESS;
}

long device_ioctl(struct file* f, unsigned int cmd, unsigned long arg)
{
    struct fsm_ioctl_struct* dftv;
    switch(cmd) {
    case FSMIOCTL_SendData:

        if(copy_from_user(&fsmdat, (void*)arg, sizeof(struct FSM_SendCmdUserspace)))
            return -EFAULT;
        dftv = FSM_FindIOCtl(fsmdat.IDDevice);
        if(dftv != 0) {
            dftv->Handler((char*)&fsmdat, sizeof(struct FSM_SendCmdUserspace), dftv);
            printk(KERN_INFO "FSM SIOCTL\n");
            printk(((struct SendSignalStruct*)fsmdat.Data)->pipe);
        }
        fsmdat.opcode = PacketToUserSpace;
        if(copy_to_user((void*)arg, &fsmdat, sizeof(struct FSM_SendCmdUserspace)))
            return -EFAULT;

        break;
    default:
        return -ENOTTY;
    }
    return 0;
}

static struct file_operations fops = { .open = device_open, .release = device_release, .unlocked_ioctl = device_ioctl };

static int __init FSM_CSIOCTLModule_init(void)
{
    if(alloc_chrdev_region(&first, 0, 1, "fsmr") < 0)
        return -1;
    if((cl = class_create(THIS_MODULE, "fsm")) == NULL) {
        unregister_chrdev_region(first, 1);
        return -1;
    }
    if(device_create(cl, NULL, first, NULL, "fsmio") == NULL) {
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return -1;
    }
    cdev_init(&c_dev, &fops);
    if(cdev_add(&c_dev, first, 1) == -1) {
        device_destroy(cl, first);
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return -1;
    }
    printk(KERN_INFO "FSM SIOCTL module loaded\n");
    return 0;
}

static void __exit FSM_CSIOCTLModule_exit(void)
{
    cdev_del(&c_dev);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);

    printk(KERN_INFO "FSM SIOCTL module unloaded\n");
}
module_init(FSM_CSIOCTLModule_init);
module_exit(FSM_CSIOCTLModule_exit);

MODULE_AUTHOR("Gusenkov S.V FSM");
MODULE_DESCRIPTION("FSM CSIOCTL Module");
MODULE_LICENSE("GPL");