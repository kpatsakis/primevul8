peer_ebgp_multihop_set_vty (struct vty *vty, const char *ip_str, 
                            const char *ttl_str)
{
  struct peer *peer;
  unsigned int ttl;

  peer = peer_and_group_lookup_vty (vty, ip_str);
  if (! peer)
    return CMD_WARNING;

  if (! ttl_str)
    ttl = TTL_MAX;
  else
    VTY_GET_INTEGER_RANGE ("TTL", ttl, ttl_str, 1, 255);

  peer_ebgp_multihop_set (peer, ttl);

  return CMD_SUCCESS;
}