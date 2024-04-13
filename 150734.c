ChangeIntegerFeedback(ClientPtr client, DeviceIntPtr dev,
                      long unsigned int mask, IntegerFeedbackPtr i,
                      xIntegerFeedbackCtl * f)
{
    if (client->swapped) {
        swaps(&f->length);
        swapl(&f->int_to_display);
    }

    i->ctrl.integer_displayed = f->int_to_display;
    (*i->CtrlProc) (dev, &i->ctrl);
    return Success;
}