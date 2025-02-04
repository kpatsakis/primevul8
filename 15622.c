peer_ebgp_multihop_unset_vty (struct vty *vty, const char *ip_str) 
{
  struct peer *peer;

  peer = peer_and_group_lookup_vty (vty, ip_str);
  if (! peer)
    return CMD_WARNING;

  peer_ebgp_multihop_unset (peer);

  return CMD_SUCCESS;
}