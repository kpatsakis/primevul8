peer_route_map_set_vty (struct vty *vty, const char *ip_str, 
                        afi_t afi, safi_t safi,
			const char *name_str, const char *direct_str)
{
  int ret;
  struct peer *peer;
  int direct = RMAP_IN;

  peer = peer_and_group_lookup_vty (vty, ip_str);
  if (! peer)
    return CMD_WARNING;

  /* Check filter direction. */
  if (strncmp (direct_str, "in", 2) == 0)
    direct = RMAP_IN;
  else if (strncmp (direct_str, "o", 1) == 0)
    direct = RMAP_OUT;
  else if (strncmp (direct_str, "im", 2) == 0)
    direct = RMAP_IMPORT;
  else if (strncmp (direct_str, "e", 1) == 0)
    direct = RMAP_EXPORT;

  ret = peer_route_map_set (peer, afi, safi, direct, name_str);

  return bgp_vty_return (vty, ret);
}