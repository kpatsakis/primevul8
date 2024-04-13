writeRandomBytes_dev_urandom(void *target, size_t count) {
  int success = 0; /* full count bytes written? */
  size_t bytesWrittenTotal = 0;

  const int fd = open("/dev/urandom", O_RDONLY);
  if (fd < 0) {
    return 0;
  }

  do {
    void *const currentTarget = (void *)((char *)target + bytesWrittenTotal);
    const size_t bytesToWrite = count - bytesWrittenTotal;

    const ssize_t bytesWrittenMore = read(fd, currentTarget, bytesToWrite);

    if (bytesWrittenMore > 0) {
      bytesWrittenTotal += bytesWrittenMore;
      if (bytesWrittenTotal >= count)
        success = 1;
    }
  } while (! success && (errno == EINTR));

  close(fd);
  return success;
}