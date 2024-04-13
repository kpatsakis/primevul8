vg_sock_fd_close(VuGpu *g)
{
    if (g->sock_fd >= 0) {
        close(g->sock_fd);
        g->sock_fd = -1;
    }
}