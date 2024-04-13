syslog_connect(void)
{
#ifdef SUNOS_5
  return -1;
#else
  int fd;
  char *s;
  struct sockaddr_un addr;

  if ((fd = socket(AF_UNIX,SOCK_DGRAM,0)) < 0)
    return -1;
  addr.sun_family = AF_UNIX;
#ifdef _PATH_LOG
#define SYSLOG_SOCKET_PATH _PATH_LOG
#else
#define SYSLOG_SOCKET_PATH "/dev/log"
#endif
  s = str_append(addr.sun_path,sizeof(addr.sun_path),SYSLOG_SOCKET_PATH);
#undef SYSLOG_SOCKET_PATH
  *s = '\0';
  if (connect(fd,(struct sockaddr *)&addr,sizeof(addr)) < 0)
    {
      close(fd);
      return -1;
    }
  return fd;
#endif
}