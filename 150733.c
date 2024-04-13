ChangeLedFeedback(ClientPtr client, DeviceIntPtr dev, long unsigned int mask,
                  LedFeedbackPtr l, xLedFeedbackCtl * f)
{
    LedCtrl lctrl;              /* might get BadValue part way through */

    if (client->swapped) {
        swaps(&f->length);
        swapl(&f->led_values);
        swapl(&f->led_mask);
    }

    f->led_mask &= l->ctrl.led_mask;    /* set only supported leds */
    f->led_values &= l->ctrl.led_mask;  /* set only supported leds */
    if (mask & DvLed) {
        lctrl.led_mask = f->led_mask;
        lctrl.led_values = f->led_values;
        (*l->CtrlProc) (dev, &lctrl);
        l->ctrl.led_values &= ~(f->led_mask);   /* zero changed leds */
        l->ctrl.led_values |= (f->led_mask & f->led_values);    /* OR in set leds */
    }

    return Success;
}