vg_destroy(VuGpu *g)
{
    struct virtio_gpu_simple_resource *res, *tmp;

    vug_deinit(&g->dev);

    vg_sock_fd_close(g);

    QTAILQ_FOREACH_SAFE(res, &g->reslist, next, tmp) {
        vg_resource_destroy(g, res);
    }

    vugbm_device_destroy(&g->gdev);
}