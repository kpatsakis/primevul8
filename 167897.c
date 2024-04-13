vg_set_features(VuDev *dev, uint64_t features)
{
    VuGpu *g = container_of(dev, VuGpu, dev.parent);
    bool virgl = features & (1 << VIRTIO_GPU_F_VIRGL);

    if (virgl && !g->virgl_inited) {
        if (!vg_virgl_init(g)) {
            vg_panic(dev, "Failed to initialize virgl");
        }
        g->virgl_inited = true;
    }

    g->virgl = virgl;
}