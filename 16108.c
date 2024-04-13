gs_deviceinitialmatrix(gx_device * dev, gs_matrix * pmat)
{
    fill_dev_proc(dev, get_initial_matrix, gx_default_get_initial_matrix);
    (*dev_proc(dev, get_initial_matrix)) (dev, pmat);
}