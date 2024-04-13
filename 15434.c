peer_timers_set_vty (struct vty *vty, const char *ip_str, 
                     const char *keep_str, const char *hold_str)
{
  int ret;
  struct peer *peer;
  u_int32_t keepalive;
  u_int32_t holdtime;

  peer = peer_and_group_lookup_vty (vty, ip_str);
  if (! peer)
    return CMD_WARNING;

  VTY_GET_INTEGER_RANGE ("Keepalive", keepalive, keep_str, 0, 65535);
  VTY_GET_INTEGER_RANGE ("Holdtime", holdtime, hold_str, 0, 65535);

  ret = peer_timers_set (peer, keepalive, holdtime);

  return bgp_vty_return (vty, ret);
}