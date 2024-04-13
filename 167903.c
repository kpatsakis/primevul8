vg_cmd_to_string(int cmd)
{
#define CMD(cmd) [cmd] = #cmd
    static const char *vg_cmd_str[] = {
        CMD(VIRTIO_GPU_UNDEFINED),

        /* 2d commands */
        CMD(VIRTIO_GPU_CMD_GET_DISPLAY_INFO),
        CMD(VIRTIO_GPU_CMD_RESOURCE_CREATE_2D),
        CMD(VIRTIO_GPU_CMD_RESOURCE_UNREF),
        CMD(VIRTIO_GPU_CMD_SET_SCANOUT),
        CMD(VIRTIO_GPU_CMD_RESOURCE_FLUSH),
        CMD(VIRTIO_GPU_CMD_TRANSFER_TO_HOST_2D),
        CMD(VIRTIO_GPU_CMD_RESOURCE_ATTACH_BACKING),
        CMD(VIRTIO_GPU_CMD_RESOURCE_DETACH_BACKING),
        CMD(VIRTIO_GPU_CMD_GET_CAPSET_INFO),
        CMD(VIRTIO_GPU_CMD_GET_CAPSET),

        /* 3d commands */
        CMD(VIRTIO_GPU_CMD_CTX_CREATE),
        CMD(VIRTIO_GPU_CMD_CTX_DESTROY),
        CMD(VIRTIO_GPU_CMD_CTX_ATTACH_RESOURCE),
        CMD(VIRTIO_GPU_CMD_CTX_DETACH_RESOURCE),
        CMD(VIRTIO_GPU_CMD_RESOURCE_CREATE_3D),
        CMD(VIRTIO_GPU_CMD_TRANSFER_TO_HOST_3D),
        CMD(VIRTIO_GPU_CMD_TRANSFER_FROM_HOST_3D),
        CMD(VIRTIO_GPU_CMD_SUBMIT_3D),

        /* cursor commands */
        CMD(VIRTIO_GPU_CMD_UPDATE_CURSOR),
        CMD(VIRTIO_GPU_CMD_MOVE_CURSOR),
    };
#undef REQ

    if (cmd >= 0 && cmd < G_N_ELEMENTS(vg_cmd_str)) {
        return vg_cmd_str[cmd];
    } else {
        return "unknown";
    }
}