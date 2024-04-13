bgp_capability_mp (struct peer *peer, struct capability_header *hdr)
{
  struct capability_mp_data mpc;
  struct stream *s = BGP_INPUT (peer);
  
  bgp_capability_mp_data (s, &mpc);
  
  if (BGP_DEBUG (normal, NORMAL))
    zlog_debug ("%s OPEN has MP_EXT CAP for afi/safi: %u/%u",
               peer->host, mpc.afi, mpc.safi);
  
  if (!bgp_afi_safi_valid_indices (mpc.afi, &mpc.safi))
    return -1;
   
  /* Now safi remapped, and afi/safi are valid array indices */
  peer->afc_recv[mpc.afi][mpc.safi] = 1;
  
  if (peer->afc[mpc.afi][mpc.safi])
    peer->afc_nego[mpc.safi][mpc.safi] = 1;
  else 
    return -1;

  return 0;
}