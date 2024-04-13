vg_set_config(VuDev *dev, const uint8_t *data,
              uint32_t offset, uint32_t size,
              uint32_t flags)
{
    VuGpu *g = container_of(dev, VuGpu, dev.parent);
    struct virtio_gpu_config *config = (struct virtio_gpu_config *)data;

    if (config->events_clear) {
        g->virtio_config.events_read &= ~config->events_clear;
    }

    return 0;
}