readn(int fd, void * buffer, size_t len)
{
  void * next = buffer;
  void * end = buffer + len;

  while (next < end)
    {
    ssize_t got = read(fd, next, end - next);

    /* I'm not sure if there are signals that can interrupt us,
    for now I assume the worst */
    if (got == -1 && errno == EINTR) continue;
    if (got <= 0) return next - buffer;
    next += got;
    }

  return len;
}