vg_process_cmd(VuGpu *vg, struct virtio_gpu_ctrl_command *cmd)
{
    switch (cmd->cmd_hdr.type) {
    case VIRTIO_GPU_CMD_GET_DISPLAY_INFO:
        vg_get_display_info(vg, cmd);
        break;
    case VIRTIO_GPU_CMD_RESOURCE_CREATE_2D:
        vg_resource_create_2d(vg, cmd);
        break;
    case VIRTIO_GPU_CMD_RESOURCE_UNREF:
        vg_resource_unref(vg, cmd);
        break;
    case VIRTIO_GPU_CMD_RESOURCE_FLUSH:
        vg_resource_flush(vg, cmd);
        break;
    case VIRTIO_GPU_CMD_TRANSFER_TO_HOST_2D:
        vg_transfer_to_host_2d(vg, cmd);
        break;
    case VIRTIO_GPU_CMD_SET_SCANOUT:
        vg_set_scanout(vg, cmd);
        break;
    case VIRTIO_GPU_CMD_RESOURCE_ATTACH_BACKING:
        vg_resource_attach_backing(vg, cmd);
        break;
    case VIRTIO_GPU_CMD_RESOURCE_DETACH_BACKING:
        vg_resource_detach_backing(vg, cmd);
        break;
    /* case VIRTIO_GPU_CMD_GET_EDID: */
    /*     break */
    default:
        g_warning("TODO handle ctrl %x\n", cmd->cmd_hdr.type);
        cmd->error = VIRTIO_GPU_RESP_ERR_UNSPEC;
        break;
    }
    if (cmd->state == VG_CMD_STATE_NEW) {
        vg_ctrl_response_nodata(vg, cmd, cmd->error ? cmd->error :
                                VIRTIO_GPU_RESP_OK_NODATA);
    }
}