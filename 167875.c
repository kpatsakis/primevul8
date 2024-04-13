vg_resource_unref(VuGpu *g,
                  struct virtio_gpu_ctrl_command *cmd)
{
    struct virtio_gpu_simple_resource *res;
    struct virtio_gpu_resource_unref unref;

    VUGPU_FILL_CMD(unref);
    virtio_gpu_bswap_32(&unref, sizeof(unref));

    res = virtio_gpu_find_resource(g, unref.resource_id);
    if (!res) {
        g_critical("%s: illegal resource specified %d",
                   __func__, unref.resource_id);
        cmd->error = VIRTIO_GPU_RESP_ERR_INVALID_RESOURCE_ID;
        return;
    }
    vg_resource_destroy(g, res);
}