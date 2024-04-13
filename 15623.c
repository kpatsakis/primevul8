peer_flag_modify_vty (struct vty *vty, const char *ip_str, 
                      u_int16_t flag, int set)
{
  int ret;
  struct peer *peer;

  peer = peer_and_group_lookup_vty (vty, ip_str);
  if (! peer)
    return CMD_WARNING;

  if (set)
    ret = peer_flag_set (peer, flag);
  else
    ret = peer_flag_unset (peer, flag);

  return bgp_vty_return (vty, ret);
}