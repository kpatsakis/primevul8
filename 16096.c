gx_device_init(gx_device * dev, const gx_device * proto, gs_memory_t * mem,
               bool internal)
{
    memcpy(dev, proto, proto->params_size);
    dev->memory = mem;
    dev->retained = !internal;
    rc_init(dev, mem, (internal ? 0 : 1));
    rc_increment(dev->icc_struct);
}