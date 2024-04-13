protocol_features_cb(gint fd, GIOCondition condition, gpointer user_data)
{
    VuGpu *g = user_data;
    uint64_t u64;
    VhostUserGpuMsg msg = {
        .request = VHOST_USER_GPU_GET_PROTOCOL_FEATURES
    };

    if (!vg_recv_msg(g, msg.request, sizeof(u64), &u64)) {
        return G_SOURCE_CONTINUE;
    }

    msg = (VhostUserGpuMsg) {
        .request = VHOST_USER_GPU_SET_PROTOCOL_FEATURES,
        .size = sizeof(uint64_t),
        .payload.u64 = 0
    };
    vg_send_msg(g, &msg, -1);

    g->wait_in = 0;
    vg_handle_ctrl(&g->dev.parent, 0);

    return G_SOURCE_REMOVE;
}