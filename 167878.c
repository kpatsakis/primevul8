vg_ctrl_response_nodata(VuGpu *g,
                        struct virtio_gpu_ctrl_command *cmd,
                        enum virtio_gpu_ctrl_type type)
{
    struct virtio_gpu_ctrl_hdr resp = {
        .type = type,
    };

    vg_ctrl_response(g, cmd, &resp, sizeof(resp));
}