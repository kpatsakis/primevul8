vg_disable_scanout(VuGpu *g, int scanout_id)
{
    struct virtio_gpu_scanout *scanout = &g->scanout[scanout_id];
    struct virtio_gpu_simple_resource *res;

    if (scanout->resource_id == 0) {
        return;
    }

    res = virtio_gpu_find_resource(g, scanout->resource_id);
    if (res) {
        res->scanout_bitmask &= ~(1 << scanout_id);
    }

    scanout->width = 0;
    scanout->height = 0;

    if (g->sock_fd >= 0) {
        VhostUserGpuMsg msg = {
            .request = VHOST_USER_GPU_SCANOUT,
            .size = sizeof(VhostUserGpuScanout),
            .payload.scanout.scanout_id = scanout_id,
        };
        vg_send_msg(g, &msg, -1);
    }
}