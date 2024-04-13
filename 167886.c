vg_recv_msg(VuGpu *g, uint32_t expect_req, uint32_t expect_size,
            gpointer payload)
{
    uint32_t req, flags, size;

    if (vg_sock_fd_read(g->sock_fd, &req, sizeof(req)) < 0 ||
        vg_sock_fd_read(g->sock_fd, &flags, sizeof(flags)) < 0 ||
        vg_sock_fd_read(g->sock_fd, &size, sizeof(size)) < 0) {
        goto err;
    }

    g_return_val_if_fail(req == expect_req, false);
    g_return_val_if_fail(flags & VHOST_USER_GPU_MSG_FLAG_REPLY, false);
    g_return_val_if_fail(size == expect_size, false);

    if (size && vg_sock_fd_read(g->sock_fd, payload, size) != size) {
        goto err;
    }

    return true;

err:
    vg_sock_fd_close(g);
    return false;
}