peer_af_flag_modify_vty (struct vty *vty, const char *peer_str, afi_t afi,
			 safi_t safi, u_int32_t flag, int set)
{
  int ret;
  struct peer *peer;

  peer = peer_and_group_lookup_vty (vty, peer_str);
  if (! peer)
    return CMD_WARNING;

  if (set)
    ret = peer_af_flag_set (peer, afi, safi, flag);
  else
    ret = peer_af_flag_unset (peer, afi, safi, flag);

  return bgp_vty_return (vty, ret);
}