vg_process_msg(VuDev *dev, VhostUserMsg *msg, int *do_reply)
{
    VuGpu *g = container_of(dev, VuGpu, dev.parent);

    switch (msg->request) {
    case VHOST_USER_GPU_SET_SOCKET: {
        g_return_val_if_fail(msg->fd_num == 1, 1);
        g_return_val_if_fail(g->sock_fd == -1, 1);
        g->sock_fd = msg->fds[0];
        set_gpu_protocol_features(g);
        return 1;
    }
    default:
        return 0;
    }

    return 0;
}