vg_get_config(VuDev *dev, uint8_t *config, uint32_t len)
{
    VuGpu *g = container_of(dev, VuGpu, dev.parent);

    if (len > sizeof(struct virtio_gpu_config)) {
        return -1;
    }

    if (opt_virgl) {
        g->virtio_config.num_capsets = vg_virgl_get_num_capsets();
    }

    memcpy(config, &g->virtio_config, len);

    return 0;
}