source_wait_cb(gint fd, GIOCondition condition, gpointer user_data)
{
    VuGpu *g = user_data;

    if (!vg_recv_msg(g, VHOST_USER_GPU_DMABUF_UPDATE, 0, NULL)) {
        return G_SOURCE_CONTINUE;
    }

    /* resume */
    g->wait_in = 0;
    vg_handle_ctrl(&g->dev.parent, 0);

    return G_SOURCE_REMOVE;
}