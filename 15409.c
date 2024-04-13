peer_af_flag_set_vty (struct vty *vty, const char *peer_str, afi_t afi,
		      safi_t safi, u_int32_t flag)
{
  return peer_af_flag_modify_vty (vty, peer_str, afi, safi, flag, 1);
}