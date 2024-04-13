gx_device_set_margins(gx_device * dev, const float *margins /*[4] */ ,
                      bool move_origin)
{
    int i;

    for (i = 0; i < 4; ++i)
        dev->HWMargins[i] = margins[i] * 72.0;
    if (move_origin) {
        dev->Margins[0] = -margins[0] * dev->HWResolution[0];
        dev->Margins[1] = -margins[3] * dev->HWResolution[1];
    }
}