set_gpu_protocol_features(VuGpu *g)
{
    VhostUserGpuMsg msg = {
        .request = VHOST_USER_GPU_GET_PROTOCOL_FEATURES
    };

    vg_send_msg(g, &msg, -1);
    assert(g->wait_in == 0);
    g->wait_in = g_unix_fd_add(g->sock_fd, G_IO_IN | G_IO_HUP,
                               protocol_features_cb, g);
}