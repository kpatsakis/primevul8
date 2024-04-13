gx_device_init_on_stack(gx_device * dev, const gx_device * proto,
                        gs_memory_t * mem)
{
    memcpy(dev, proto, proto->params_size);
    dev->memory = mem;
    dev->retained = 0;
    dev->pad = proto->pad;
    dev->log2_align_mod = proto->log2_align_mod;
    dev->is_planar = proto->is_planar;
    rc_init(dev, NULL, 0);
}