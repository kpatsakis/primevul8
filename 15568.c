peer_advertise_interval_vty (struct vty *vty, const char *ip_str, 
                             const char *time_str, int set)  
{
  int ret;
  struct peer *peer;
  u_int32_t routeadv = 0;

  peer = peer_lookup_vty (vty, ip_str);
  if (! peer)
    return CMD_WARNING;

  if (time_str)
    VTY_GET_INTEGER_RANGE ("advertise interval", routeadv, time_str, 0, 600);

  if (set)
    ret = peer_advertise_interval_set (peer, routeadv);
  else
    ret = peer_advertise_interval_unset (peer);

  return CMD_SUCCESS;
}