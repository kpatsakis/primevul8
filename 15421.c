peer_flag_unset_vty (struct vty *vty, const char *ip_str, u_int16_t flag)
{
  return peer_flag_modify_vty (vty, ip_str, flag, 0);
}