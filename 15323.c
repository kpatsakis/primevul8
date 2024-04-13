peer_timers_connect_set_vty (struct vty *vty, const char *ip_str, 
                             const char *time_str)
{
  int ret;
  struct peer *peer;
  u_int32_t connect;

  peer = peer_lookup_vty (vty, ip_str);
  if (! peer)
    return CMD_WARNING;

  VTY_GET_INTEGER_RANGE ("Connect time", connect, time_str, 0, 65535);

  ret = peer_timers_connect_set (peer, connect);

  return CMD_SUCCESS;
}