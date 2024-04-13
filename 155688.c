writeRandomBytes_getrandom_nonblock(void *target, size_t count) {
  int success = 0; /* full count bytes written? */
  size_t bytesWrittenTotal = 0;
  const unsigned int getrandomFlags = GRND_NONBLOCK;

  do {
    void *const currentTarget = (void *)((char *)target + bytesWrittenTotal);
    const size_t bytesToWrite = count - bytesWrittenTotal;

    const int bytesWrittenMore =
#    if defined(HAVE_GETRANDOM)
        getrandom(currentTarget, bytesToWrite, getrandomFlags);
#    else
        syscall(SYS_getrandom, currentTarget, bytesToWrite, getrandomFlags);
#    endif

    if (bytesWrittenMore > 0) {
      bytesWrittenTotal += bytesWrittenMore;
      if (bytesWrittenTotal >= count)
        success = 1;
    }
  } while (! success && (errno == EINTR));

  return success;
}