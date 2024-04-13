vg_send_msg(VuGpu *vg, const VhostUserGpuMsg *msg, int fd)
{
    if (vg_sock_fd_write(vg->sock_fd, msg,
                         VHOST_USER_GPU_HDR_SIZE + msg->size, fd) < 0) {
        vg_sock_fd_close(vg);
    }
}