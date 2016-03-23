/*!
\file
\brief Набор данных FSM Аудиоустроство
\authors Гусенков.С.В
\version 0.0.1_rc1
\date 30.12.2015
*/
#ifndef FCM_AUDIODEVICECLASS_H
#define	FCM_AUDIODEVICECLASS_H
/*!
\brief Вид устроства
*/
enum FSMAD_VidDevice
{
    CommunicationDevice=1 ///< Устроства связи
};
/*!
\brief ПодВид устроства
*/
enum FSMAD_PodVidDevice
{
   CCK=1 ///< СЦК
};
/*!
\brief Род устроства
*/
enum FSMAD_RodDevice
{
    MN921=1,///< MN921
    MN825=2,///< MN825
    PO07=3,///< PO07
    PO08=4,///< PO08
    PO06=5,///< PO06
    MN524=6,///< MN524
};
#endif	/* FCM_AUDIODEVICECLASS_H */
