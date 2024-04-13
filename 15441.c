peer_unsuppress_map_set_vty (struct vty *vty, const char *ip_str, afi_t afi,
			     safi_t safi, const char *name_str)
{
  int ret;
  struct peer *peer;

  peer = peer_and_group_lookup_vty (vty, ip_str);
  if (! peer)
    return CMD_WARNING;

  ret = peer_unsuppress_map_set (peer, afi, safi, name_str);

  return bgp_vty_return (vty, ret);
}