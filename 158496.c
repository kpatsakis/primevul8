tfo_out_check(int sock)
{
# if defined(TCP_INFO) && defined(EXIM_HAVE_TCPI_UNACKED)
struct tcp_info tinfo;
socklen_t len = sizeof(tinfo);

switch (tcp_out_fastopen)
  {
    /* This is a somewhat dubious detection method; totally undocumented so likely
    to fail in future kernels.  There seems to be no documented way.  What we really
    want to know is if the server sent smtp-banner data before our ACK of his SYN,ACK
    hit him.  What this (possibly?) detects is whether we sent a TFO cookie with our
    SYN, as distinct from a TFO request.  This gets a false-positive when the server
    key is rotated; we send the old one (which this test sees) but the server returns
    the new one and does not send its SMTP banner before we ACK his SYN,ACK.
     To force that rotation case:
     '# echo -n "00000000-00000000-00000000-0000000" >/proc/sys/net/ipv4/tcp_fastopen_key'
    The kernel seems to be counting unack'd packets. */

  case TFO_ATTEMPTED_NODATA:
    if (  getsockopt(sock, IPPROTO_TCP, TCP_INFO, &tinfo, &len) == 0
       && tinfo.tcpi_state == TCP_SYN_SENT
       && tinfo.tcpi_unacked > 1
       )
      {
      DEBUG(D_transport|D_v)
	debug_printf("TCP_FASTOPEN tcpi_unacked %d\n", tinfo.tcpi_unacked);
      tcp_out_fastopen = TFO_USED_NODATA;
      }
    break;

    /* When called after waiting for received data we should be able
    to tell if data we sent was accepted. */

  case TFO_ATTEMPTED_DATA:
    if (  getsockopt(sock, IPPROTO_TCP, TCP_INFO, &tinfo, &len) == 0
       && tinfo.tcpi_state == TCP_ESTABLISHED
       )
      if (tinfo.tcpi_options & TCPI_OPT_SYN_DATA)
	{
	DEBUG(D_transport|D_v) debug_printf("TFO: data was acked\n");
	tcp_out_fastopen = TFO_USED_DATA;
	}
      else
	{
	DEBUG(D_transport|D_v) debug_printf("TFO: had to retransmit\n");
	tcp_out_fastopen = TFO_NOT_USED;
	}
    break;
  }
# endif
}