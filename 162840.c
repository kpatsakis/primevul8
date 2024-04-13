set_qos_dscp (GSocket * socket, guint qos_dscp)
{
#ifndef IP_TOS
  GST_FIXME ("IP_TOS socket option is not defined, not setting dscp");
  return GST_RTSP_OK;
#else
  gint fd;
  union gst_sockaddr sa;
  socklen_t slen = sizeof (sa);
  gint af;
  gint tos;

  if (!socket)
    return GST_RTSP_OK;

  fd = g_socket_get_fd (socket);
  if (getsockname (fd, &sa.sa, &slen) < 0)
    goto no_getsockname;

  af = sa.sa.sa_family;

  /* if this is an IPv4-mapped address then do IPv4 QoS */
  if (af == AF_INET6) {
    if (IN6_IS_ADDR_V4MAPPED (&sa.sa_in6.sin6_addr))
      af = AF_INET;
  }

  /* extract and shift 6 bits of the DSCP */
  tos = (qos_dscp & 0x3f) << 2;

#ifdef G_OS_WIN32
#  define SETSOCKOPT_ARG4_TYPE const char *
#else
#  define SETSOCKOPT_ARG4_TYPE const void *
#endif

  switch (af) {
    case AF_INET:
      if (setsockopt (fd, IPPROTO_IP, IP_TOS, (SETSOCKOPT_ARG4_TYPE) & tos,
              sizeof (tos)) < 0)
        goto no_setsockopt;
      break;
    case AF_INET6:
#ifdef IPV6_TCLASS
      if (setsockopt (fd, IPPROTO_IPV6, IPV6_TCLASS,
              (SETSOCKOPT_ARG4_TYPE) & tos, sizeof (tos)) < 0)
        goto no_setsockopt;
      break;
#endif
    default:
      goto wrong_family;
  }

  return GST_RTSP_OK;

  /* ERRORS */
no_getsockname:
no_setsockopt:
  {
    return GST_RTSP_ESYS;
  }
wrong_family:
  {
    return GST_RTSP_ERROR;
  }
#endif
}