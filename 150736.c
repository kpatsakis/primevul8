ChangeBellFeedback(ClientPtr client, DeviceIntPtr dev,
                   long unsigned int mask, BellFeedbackPtr b,
                   xBellFeedbackCtl * f)
{
    int t;
    BellCtrl bctrl;             /* might get BadValue part way through */

    if (client->swapped) {
        swaps(&f->length);
        swaps(&f->pitch);
        swaps(&f->duration);
    }

    bctrl = b->ctrl;
    if (mask & DvPercent) {
        t = f->percent;
        if (t == -1)
            t = defaultKeyboardControl.bell;
        else if (t < 0 || t > 100) {
            client->errorValue = t;
            return BadValue;
        }
        bctrl.percent = t;
    }

    if (mask & DvPitch) {
        t = f->pitch;
        if (t == -1)
            t = defaultKeyboardControl.bell_pitch;
        else if (t < 0) {
            client->errorValue = t;
            return BadValue;
        }
        bctrl.pitch = t;
    }

    if (mask & DvDuration) {
        t = f->duration;
        if (t == -1)
            t = defaultKeyboardControl.bell_duration;
        else if (t < 0) {
            client->errorValue = t;
            return BadValue;
        }
        bctrl.duration = t;
    }
    b->ctrl = bctrl;
    (*b->CtrlProc) (dev, &b->ctrl);
    return Success;
}