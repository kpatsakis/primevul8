get_display_info_cb(gint fd, GIOCondition condition, gpointer user_data)
{
    struct virtio_gpu_resp_display_info dpy_info = { {} };
    VuGpu *vg = user_data;
    struct virtio_gpu_ctrl_command *cmd = QTAILQ_LAST(&vg->fenceq);

    g_debug("disp info cb");
    assert(cmd->cmd_hdr.type == VIRTIO_GPU_CMD_GET_DISPLAY_INFO);
    if (!vg_recv_msg(vg, VHOST_USER_GPU_GET_DISPLAY_INFO,
                     sizeof(dpy_info), &dpy_info)) {
        return G_SOURCE_CONTINUE;
    }

    QTAILQ_REMOVE(&vg->fenceq, cmd, next);
    vg_ctrl_response(vg, cmd, &dpy_info.hdr, sizeof(dpy_info));

    vg->wait_in = 0;
    vg_handle_ctrl(&vg->dev.parent, 0);

    return G_SOURCE_REMOVE;
}