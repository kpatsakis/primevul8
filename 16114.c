gs_closedevice(gx_device * dev)
{
    int code = 0;

    if (dev->is_open) {
        code = (*dev_proc(dev, close_device))(dev);
        dev->is_open = false;
        if (code < 0)
            return_error(code);
    }
    return code;
}