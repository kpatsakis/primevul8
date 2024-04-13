gs_copyscanlines(gx_device * dev, int start_y, byte * data, uint size,
                 int *plines_copied, uint * pbytes_copied)
{
    uint line_size = gx_device_raster(dev, 0);
    uint count = size / line_size;
    uint i;
    byte *dest = data;

    for (i = 0; i < count; i++, dest += line_size) {
        int code = (*dev_proc(dev, get_bits)) (dev, start_y + i, dest, NULL);

        if (code < 0) {
            /* Might just be an overrun. */
            if (start_y + i == dev->height)
                break;
            return_error(code);
        }
    }
    if (plines_copied != NULL)
        *plines_copied = i;
    if (pbytes_copied != NULL)
        *pbytes_copied = i * line_size;
    return 0;
}