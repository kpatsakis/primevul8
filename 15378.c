peer_timers_connect_unset_vty (struct vty *vty, const char *ip_str)
{
  int ret;
  struct peer *peer;

  peer = peer_and_group_lookup_vty (vty, ip_str);
  if (! peer)
    return CMD_WARNING;

  ret = peer_timers_connect_unset (peer);

  return CMD_SUCCESS;
}