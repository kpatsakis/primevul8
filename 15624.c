peer_default_originate_set_vty (struct vty *vty, const char *peer_str, 
                                afi_t afi, safi_t safi, 
                                const char *rmap, int set)
{
  int ret;
  struct peer *peer;

  peer = peer_and_group_lookup_vty (vty, peer_str);
  if (! peer)
    return CMD_WARNING;

  if (set)
    ret = peer_default_originate_set (peer, afi, safi, rmap);
  else
    ret = peer_default_originate_unset (peer, afi, safi);

  return bgp_vty_return (vty, ret);
}