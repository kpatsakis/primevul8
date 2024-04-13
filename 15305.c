peer_timers_unset_vty (struct vty *vty, const char *ip_str)
{
  int ret;
  struct peer *peer;

  peer = peer_lookup_vty (vty, ip_str);
  if (! peer)
    return CMD_WARNING;

  ret = peer_timers_unset (peer);

  return bgp_vty_return (vty, ret);
}