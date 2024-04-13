bgp_notify_send_with_data (struct peer *peer, u_char code, u_char sub_code,
			   u_char *data, size_t datalen)
{
  struct stream *s;
  int length;

  /* Allocate new stream. */
  s = stream_new (BGP_MAX_PACKET_SIZE);

  /* Make nitify packet. */
  bgp_packet_set_marker (s, BGP_MSG_NOTIFY);

  /* Set notify packet values. */
  stream_putc (s, code);        /* BGP notify code */
  stream_putc (s, sub_code);	/* BGP notify sub_code */

  /* If notify data is present. */
  if (data)
    stream_write (s, data, datalen);
  
  /* Set BGP packet length. */
  length = bgp_packet_set_size (s);
  
  /* Add packet to the peer. */
  stream_fifo_clean (peer->obuf);
  bgp_packet_add (peer, s);

  /* For debug */
  {
    struct bgp_notify bgp_notify;
    int first = 0;
    int i;
    char c[4];

    bgp_notify.code = code;
    bgp_notify.subcode = sub_code;
    bgp_notify.data = NULL;
    bgp_notify.length = length - BGP_MSG_NOTIFY_MIN_SIZE;
    
    if (bgp_notify.length)
      {
	bgp_notify.data = XMALLOC (MTYPE_TMP, bgp_notify.length * 3);
	for (i = 0; i < bgp_notify.length; i++)
	  if (first)
	    {
	      sprintf (c, " %02x", data[i]);
	      strcat (bgp_notify.data, c);
	    }
	  else
	    {
	      first = 1;
	      sprintf (c, "%02x", data[i]);
	      strcpy (bgp_notify.data, c);
	    }
      }
    bgp_notify_print (peer, &bgp_notify, "sending");
    if (bgp_notify.data)
      XFREE (MTYPE_TMP, bgp_notify.data);
  }

  if (BGP_DEBUG (normal, NORMAL))
    zlog_debug ("%s send message type %d, length (incl. header) %d",
	       peer->host, BGP_MSG_NOTIFY, length);

  /* peer reset cause */
  if (sub_code != BGP_NOTIFY_CEASE_CONFIG_CHANGE)
    {
      if (sub_code == BGP_NOTIFY_CEASE_ADMIN_RESET)
      peer->last_reset = PEER_DOWN_USER_RESET;
      else if (sub_code == BGP_NOTIFY_CEASE_ADMIN_SHUTDOWN)
      peer->last_reset = PEER_DOWN_USER_SHUTDOWN;
      else
      peer->last_reset = PEER_DOWN_NOTIFY_SEND;
    }

  /* Call imidiately. */
  BGP_WRITE_OFF (peer->t_write);

  bgp_write_notify (peer);
}