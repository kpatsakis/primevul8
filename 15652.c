peer_update_source_vty (struct vty *vty, const char *peer_str, 
                        const char *source_str)
{
  struct peer *peer;
  union sockunion *su;

  peer = peer_and_group_lookup_vty (vty, peer_str);
  if (! peer)
    return CMD_WARNING;

  if (source_str)
    {
      su = sockunion_str2su (source_str);
      if (su)
	{
	  peer_update_source_addr_set (peer, su);
	  sockunion_free (su);
	}
      else
	peer_update_source_if_set (peer, source_str);
    }
  else
    peer_update_source_unset (peer);

  return CMD_SUCCESS;
}