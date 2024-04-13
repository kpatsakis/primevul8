bgp_notify_receive (struct peer *peer, bgp_size_t size)
{
  struct bgp_notify bgp_notify;

  if (peer->notify.data)
    {
      XFREE (MTYPE_TMP, peer->notify.data);
      peer->notify.data = NULL;
      peer->notify.length = 0;
    }

  bgp_notify.code = stream_getc (peer->ibuf);
  bgp_notify.subcode = stream_getc (peer->ibuf);
  bgp_notify.length = size - 2;
  bgp_notify.data = NULL;

  /* Preserv notify code and sub code. */
  peer->notify.code = bgp_notify.code;
  peer->notify.subcode = bgp_notify.subcode;
  /* For further diagnostic record returned Data. */
  if (bgp_notify.length)
    {
      peer->notify.length = size - 2;
      peer->notify.data = XMALLOC (MTYPE_TMP, size - 2);
      memcpy (peer->notify.data, stream_pnt (peer->ibuf), size - 2);
    }

  /* For debug */
  {
    int i;
    int first = 0;
    char c[4];

    if (bgp_notify.length)
      {
	bgp_notify.data = XMALLOC (MTYPE_TMP, bgp_notify.length * 3);
	for (i = 0; i < bgp_notify.length; i++)
	  if (first)
	    {
	      sprintf (c, " %02x", stream_getc (peer->ibuf));
	      strcat (bgp_notify.data, c);
	    }
	  else
	    {
	      first = 1;
	      sprintf (c, "%02x", stream_getc (peer->ibuf));
	      strcpy (bgp_notify.data, c);
	    }
      }

    bgp_notify_print(peer, &bgp_notify, "received");
    if (bgp_notify.data)
      XFREE (MTYPE_TMP, bgp_notify.data);
  }

  /* peer count update */
  peer->notify_in++;

  if (peer->status == Established)
    peer->last_reset = PEER_DOWN_NOTIFY_RECEIVED;

  /* We have to check for Notify with Unsupported Optional Parameter.
     in that case we fallback to open without the capability option.
     But this done in bgp_stop. We just mark it here to avoid changing
     the fsm tables.  */
  if (bgp_notify.code == BGP_NOTIFY_OPEN_ERR &&
      bgp_notify.subcode == BGP_NOTIFY_OPEN_UNSUP_PARAM )
    UNSET_FLAG (peer->sflags, PEER_STATUS_CAPABILITY_OPEN);

  /* Also apply to Unsupported Capability until remote router support
     capability. */
  if (bgp_notify.code == BGP_NOTIFY_OPEN_ERR &&
      bgp_notify.subcode == BGP_NOTIFY_OPEN_UNSUP_CAPBL)
    UNSET_FLAG (peer->sflags, PEER_STATUS_CAPABILITY_OPEN);

  BGP_EVENT_ADD (peer, Receive_NOTIFICATION_message);
}