peer_maximum_prefix_set_vty (struct vty *vty, const char *ip_str, afi_t afi,
			     safi_t safi, const char *num_str,  
			     const char *threshold_str, int warning,
			     const char *restart_str)
{
  int ret;
  struct peer *peer;
  u_int32_t max;
  u_char threshold;
  u_int16_t restart;

  peer = peer_and_group_lookup_vty (vty, ip_str);
  if (! peer)
    return CMD_WARNING;

  VTY_GET_INTEGER ("maxmum number", max, num_str);
  if (threshold_str)
    threshold = atoi (threshold_str);
  else
    threshold = MAXIMUM_PREFIX_THRESHOLD_DEFAULT;

  if (restart_str)
    restart = atoi (restart_str);
  else
    restart = 0;

  ret = peer_maximum_prefix_set (peer, afi, safi, max, threshold, warning, restart);

  return bgp_vty_return (vty, ret);
}