vg_resource_detach_backing(VuGpu *g,
                           struct virtio_gpu_ctrl_command *cmd)
{
    struct virtio_gpu_simple_resource *res;
    struct virtio_gpu_resource_detach_backing detach;

    VUGPU_FILL_CMD(detach);
    virtio_gpu_bswap_32(&detach, sizeof(detach));

    res = virtio_gpu_find_resource(g, detach.resource_id);
    if (!res || !res->iov) {
        g_critical("%s: illegal resource specified %d",
                   __func__, detach.resource_id);
        cmd->error = VIRTIO_GPU_RESP_ERR_INVALID_RESOURCE_ID;
        return;
    }

    g_free(res->iov);
    res->iov = NULL;
    res->iov_cnt = 0;
}