#include "kmclass.h"
#include "global.h"
#include "dbg_r3.h"

void  Mouse_noirp(PSHM_CONTEXT context, USHORT Flags)
{


    if (context->uInit != TRUE)
        return;
    PMOUSE_INPUT_DATA  mid = (PMOUSE_INPUT_DATA)(&context->dataSubmit[0]);

    mid->ButtonFlags = Flags;

    context->cmd = CMD_MOUSE;
    dcptr->kSendCmd(context);
    dcptr->kCleanUp(context);
}
//MOUSE_MOVE_RELATIVE  MOUSE_MOVE_ABSOLUTE
void  MouseMove_noirp(PSHM_CONTEXT context, USHORT tp, LONG dx, LONG dy)
{
    if (context->uInit != TRUE) {
        Ot("$ MouseMoveRELATIVE_noirp err 9\n");
        return;
    }
    PMOUSE_INPUT_DATA  mid = (PMOUSE_INPUT_DATA)(&context->dataSubmit[0]);
    mid->Flags = tp;
    mid->LastX = dx;
    mid->LastY = dy;
    context->cmd = CMD_MOUSE;
    dcptr->kSendCmd(context);
    dcptr->kCleanUp(context);
}


void  KeyDown_noirp(PSHM_CONTEXT context, USHORT VirtualKey)
{
    if (context->uInit != TRUE) {
        Ot("$ KeyDown_noirp err 23\n");
        return;
    }


    PKEYBOARD_INPUT_DATA  kid = (PKEYBOARD_INPUT_DATA)(&context->dataSubmit[0]);
    RtlZeroMemory(kid, sizeof(KEYBOARD_INPUT_DATA));

    kid->Flags = KEY_DOWN;
    //kid->MakeCode = (USHORT)MapVirtualKey(VirtualKey, 0);
    kid->MakeCode = VirtualKey;



    context->cmd = CMD_KBD;
    dcptr->kSendCmd(context);
    dcptr->kCleanUp(context);
}


void  KeyUp_noirp(PSHM_CONTEXT context, USHORT VirtualKey)
{
    if (context->uInit != TRUE) {
        Ot("$ KeyUp_noirp err 48\n");
        return;
    }

    PKEYBOARD_INPUT_DATA  kid = (PKEYBOARD_INPUT_DATA)(&context->dataSubmit[0]);
    RtlZeroMemory(kid, sizeof(KEYBOARD_INPUT_DATA));

    kid->Flags = KEY_UP;
    //kid->MakeCode = (USHORT)MapVirtualKey(VirtualKey, 0);
    kid->MakeCode = VirtualKey;



    context->cmd = CMD_KBD;
    dcptr->kSendCmd(context);
    dcptr->kCleanUp(context);
}
