tfo_in_check(void)
{
# ifdef TCP_INFO
struct tcp_info tinfo;
socklen_t len = sizeof(tinfo);

if (getsockopt(fileno(smtp_out), IPPROTO_TCP, TCP_INFO, &tinfo, &len) == 0)
#ifdef TCPI_OPT_SYN_DATA	/* FreeBSD 11 does not seem to have this yet */
  if (tinfo.tcpi_options & TCPI_OPT_SYN_DATA)
    {
    DEBUG(D_receive) debug_printf("TCP_FASTOPEN mode connection (ACKd data-on-SYN)\n");
    f.tcp_in_fastopen_data = f.tcp_in_fastopen = TRUE;
    }
  else
#endif
    if (tinfo.tcpi_state == TCP_SYN_RECV)
    {
    DEBUG(D_receive) debug_printf("TCP_FASTOPEN mode connection (state TCP_SYN_RECV)\n");
    f.tcp_in_fastopen = TRUE;
    }
# endif
}