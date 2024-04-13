bgp_write (struct thread *thread)
{
  struct peer *peer;
  u_char type;
  struct stream *s; 
  int num;
  unsigned int count = 0;
  int write_errno;

  /* Yes first of all get peer pointer. */
  peer = THREAD_ARG (thread);
  peer->t_write = NULL;

  /* For non-blocking IO check. */
  if (peer->status == Connect)
    {
      bgp_connect_check (peer);
      return 0;
    }

    /* Nonblocking write until TCP output buffer is full.  */
  while (1)
    {
      int writenum;
      int val;

      s = bgp_write_packet (peer);
      if (! s)
	return 0;
      
      /* XXX: FIXME, the socket should be NONBLOCK from the start
       * status shouldnt need to be toggled on each write
       */
      val = fcntl (peer->fd, F_GETFL, 0);
      fcntl (peer->fd, F_SETFL, val|O_NONBLOCK);

      /* Number of bytes to be sent.  */
      writenum = stream_get_endp (s) - stream_get_getp (s);

      /* Call write() system call.  */
      num = write (peer->fd, STREAM_PNT (s), writenum);
      write_errno = errno;
      fcntl (peer->fd, F_SETFL, val);
      if (num <= 0)
	{
	  /* Partial write. */
	  if (write_errno == EWOULDBLOCK || write_errno == EAGAIN)
	      break;

	  BGP_EVENT_ADD (peer, TCP_fatal_error);
	  return 0;
	}
      if (num != writenum)
	{
	  stream_forward_getp (s, num);

	  if (write_errno == EAGAIN)
	    break;

	  continue;
	}

      /* Retrieve BGP packet type. */
      stream_set_getp (s, BGP_MARKER_SIZE + 2);
      type = stream_getc (s);

      switch (type)
	{
	case BGP_MSG_OPEN:
	  peer->open_out++;
	  break;
	case BGP_MSG_UPDATE:
	  peer->update_out++;
	  break;
	case BGP_MSG_NOTIFY:
	  peer->notify_out++;
	  /* Double start timer. */
	  peer->v_start *= 2;

	  /* Overflow check. */
	  if (peer->v_start >= (60 * 2))
	    peer->v_start = (60 * 2);

	  /* Flush any existing events */
	  BGP_EVENT_ADD (peer, BGP_Stop);
	  return 0;
	case BGP_MSG_KEEPALIVE:
	  peer->keepalive_out++;
	  break;
	case BGP_MSG_ROUTE_REFRESH_NEW:
	case BGP_MSG_ROUTE_REFRESH_OLD:
	  peer->refresh_out++;
	  break;
	case BGP_MSG_CAPABILITY:
	  peer->dynamic_cap_out++;
	  break;
	}

      /* OK we send packet so delete it. */
      bgp_packet_delete (peer);

      if (++count >= BGP_WRITE_PACKET_MAX)
	break;
    }
  
  if (bgp_write_proceed (peer))
    BGP_WRITE_ON (peer->t_write, bgp_write, peer->fd);
  
  return 0;
}