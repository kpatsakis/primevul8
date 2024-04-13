gs_opendevice(gx_device *dev)
{
    if (dev->is_open)
        return 0;
    check_device_separable(dev);
    gx_device_fill_in_procs(dev);
    {
        int code = (*dev_proc(dev, open_device))(dev);

        if (code < 0)
            return_error(code);
        dev->is_open = true;
        return 1;
    }
}