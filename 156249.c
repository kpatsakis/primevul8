hivex_commit (hive_h *h, const char *filename, int flags)
{
  int fd;

  if (flags != 0) {
    SET_ERRNO (EINVAL, "flags != 0");
    return -1;
  }

  CHECK_WRITABLE (-1);

  filename = filename ? : h->filename;
#ifdef O_CLOEXEC
  fd = open (filename, O_WRONLY|O_CREAT|O_TRUNC|O_NOCTTY|O_CLOEXEC|O_BINARY,
             0666);
#else
  fd = open (filename, O_WRONLY|O_CREAT|O_TRUNC|O_NOCTTY|O_BINARY, 0666);
#endif
  if (fd == -1)
    return -1;
#ifndef O_CLOEXEC
  fcntl (fd, F_SETFD, FD_CLOEXEC);
#endif

  /* Update the header fields. */
  uint32_t sequence = le32toh (h->hdr->sequence1);
  sequence++;
  h->hdr->sequence1 = htole32 (sequence);
  h->hdr->sequence2 = htole32 (sequence);
  /* XXX Ought to update h->hdr->last_modified. */
  h->hdr->blocks = htole32 (h->endpages - 0x1000);

  /* Recompute header checksum. */
  uint32_t sum = header_checksum (h);
  h->hdr->csum = htole32 (sum);

  DEBUG (2, "hivex_commit: new header checksum: 0x%x", sum);

  if (full_write (fd, h->addr, h->size) != h->size) {
    int err = errno;
    close (fd);
    errno = err;
    return -1;
  }

  if (close (fd) == -1)
    return -1;

  return 0;
}