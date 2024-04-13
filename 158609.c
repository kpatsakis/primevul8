write_to_fd_buf(int fd, const uschar *buf, size_t length)
{
ssize_t wrote;
size_t total_written = 0;
const uschar *p = buf;
size_t left = length;

while (1)
  {
  wrote = write(fd, p, left);
  if (wrote == (ssize_t)-1)
    {
    if (errno == EINTR) continue;
    return wrote;
    }
  total_written += wrote;
  if (wrote == left)
    break;
  else
    {
    p += wrote;
    left -= wrote;
    }
  }
return total_written;
}