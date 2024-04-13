peer_maximum_prefix_unset_vty (struct vty *vty, const char *ip_str, afi_t afi,
			       safi_t safi)
{
  int ret;
  struct peer *peer;

  peer = peer_and_group_lookup_vty (vty, ip_str);
  if (! peer)
    return CMD_WARNING;

  ret = peer_maximum_prefix_unset (peer, afi, safi);

  return bgp_vty_return (vty, ret);
}