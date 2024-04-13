peer_distribute_set_vty (struct vty *vty, const char *ip_str, 
                         afi_t afi, safi_t safi,
			 const char *name_str, const char *direct_str)
{
  int ret;
  struct peer *peer;
  int direct = FILTER_IN;

  peer = peer_and_group_lookup_vty (vty, ip_str);
  if (! peer)
    return CMD_WARNING;

  /* Check filter direction. */
  if (strncmp (direct_str, "i", 1) == 0)
    direct = FILTER_IN;
  else if (strncmp (direct_str, "o", 1) == 0)
    direct = FILTER_OUT;

  ret = peer_distribute_set (peer, afi, safi, direct, name_str);

  return bgp_vty_return (vty, ret);
}