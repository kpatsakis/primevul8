get_pf_socket(void)
{
  int pf;
  /*  This should be opened before dropping privileges. */
  if (pf_socket >= 0)
    return pf_socket;

#if defined(OpenBSD)
  /* only works on OpenBSD */
  pf = tor_open_cloexec("/dev/pf", O_RDONLY, 0);
#else
  /* works on NetBSD and FreeBSD */
  pf = tor_open_cloexec("/dev/pf", O_RDWR, 0);
#endif /* defined(OpenBSD) */

  if (pf < 0) {
    log_warn(LD_NET, "open(\"/dev/pf\") failed: %s", strerror(errno));
    return -1;
  }

  pf_socket = pf;
  return pf_socket;
}