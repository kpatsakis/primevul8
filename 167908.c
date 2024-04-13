vg_get_features(VuDev *dev)
{
    uint64_t features = 0;

    if (opt_virgl) {
        features |= 1 << VIRTIO_GPU_F_VIRGL;
    }

    return features;
}