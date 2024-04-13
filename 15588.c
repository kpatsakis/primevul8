bgp_read_packet (struct peer *peer)
{
  int nbytes;
  int readsize;

  readsize = peer->packet_size - stream_get_endp (peer->ibuf);

  /* If size is zero then return. */
  if (! readsize)
    return 0;

  /* Read packet from fd. */
  nbytes = stream_read_unblock (peer->ibuf, peer->fd, readsize);

  /* If read byte is smaller than zero then error occured. */
  if (nbytes < 0) 
    {
      if (errno == EAGAIN)
	return -1;

      plog_err (peer->log, "%s [Error] bgp_read_packet error: %s",
		 peer->host, safe_strerror (errno));

      if (peer->status == Established) 
	{
	  if (CHECK_FLAG (peer->sflags, PEER_STATUS_NSF_MODE))
	    {
	      peer->last_reset = PEER_DOWN_NSF_CLOSE_SESSION;
	      SET_FLAG (peer->sflags, PEER_STATUS_NSF_WAIT);
	    }
	  else
	    peer->last_reset = PEER_DOWN_CLOSE_SESSION;
	}

      BGP_EVENT_ADD (peer, TCP_fatal_error);
      return -1;
    }  

  /* When read byte is zero : clear bgp peer and return */
  if (nbytes == 0) 
    {
      if (BGP_DEBUG (events, EVENTS))
	plog_debug (peer->log, "%s [Event] BGP connection closed fd %d",
		   peer->host, peer->fd);

      if (peer->status == Established) 
	{
	  if (CHECK_FLAG (peer->sflags, PEER_STATUS_NSF_MODE))
	    {
	      peer->last_reset = PEER_DOWN_NSF_CLOSE_SESSION;
	      SET_FLAG (peer->sflags, PEER_STATUS_NSF_WAIT);
	    }
	  else
	    peer->last_reset = PEER_DOWN_CLOSE_SESSION;
	}

      BGP_EVENT_ADD (peer, TCP_connection_closed);
      return -1;
    }

  /* We read partial packet. */
  if (stream_get_endp (peer->ibuf) != peer->packet_size)
    return -1;

  return 0;
}