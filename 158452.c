restore_socket_timeout(int fd, int get_ok, struct timeval * tvtmp, socklen_t vslen)
{
if (get_ok == 0)
  (void) setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, CS tvtmp, vslen);
}