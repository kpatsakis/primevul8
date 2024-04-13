vg_get_display_info(VuGpu *vg, struct virtio_gpu_ctrl_command *cmd)
{
    VhostUserGpuMsg msg = {
        .request = VHOST_USER_GPU_GET_DISPLAY_INFO,
        .size = 0,
    };

    assert(vg->wait_in == 0);

    vg_send_msg(vg, &msg, -1);
    vg->wait_in = g_unix_fd_add(vg->sock_fd, G_IO_IN | G_IO_HUP,
                               get_display_info_cb, vg);
    cmd->state = VG_CMD_STATE_PENDING;
}