vg_wait_ok(VuGpu *g)
{
    assert(g->wait_in == 0);
    g->wait_in = g_unix_fd_add(g->sock_fd, G_IO_IN | G_IO_HUP,
                               source_wait_cb, g);
}