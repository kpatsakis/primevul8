ChangePtrFeedback(ClientPtr client, DeviceIntPtr dev, long unsigned int mask,
                  PtrFeedbackPtr p, xPtrFeedbackCtl * f)
{
    PtrCtrl pctrl;              /* might get BadValue part way through */

    if (client->swapped) {
        swaps(&f->length);
        swaps(&f->num);
        swaps(&f->denom);
        swaps(&f->thresh);
    }

    pctrl = p->ctrl;
    if (mask & DvAccelNum) {
        int accelNum;

        accelNum = f->num;
        if (accelNum == -1)
            pctrl.num = defaultPointerControl.num;
        else if (accelNum < 0) {
            client->errorValue = accelNum;
            return BadValue;
        }
        else
            pctrl.num = accelNum;
    }

    if (mask & DvAccelDenom) {
        int accelDenom;

        accelDenom = f->denom;
        if (accelDenom == -1)
            pctrl.den = defaultPointerControl.den;
        else if (accelDenom <= 0) {
            client->errorValue = accelDenom;
            return BadValue;
        }
        else
            pctrl.den = accelDenom;
    }

    if (mask & DvThreshold) {
        int threshold;

        threshold = f->thresh;
        if (threshold == -1)
            pctrl.threshold = defaultPointerControl.threshold;
        else if (threshold < 0) {
            client->errorValue = threshold;
            return BadValue;
        }
        else
            pctrl.threshold = threshold;
    }

    p->ctrl = pctrl;
    (*p->CtrlProc) (dev, &p->ctrl);
    return Success;
}