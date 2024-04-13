bgp_capability_restart (struct peer *peer, struct capability_header *caphdr)
{
  struct stream *s = BGP_INPUT (peer);
  u_int16_t restart_flag_time;
  int restart_bit = 0;
  size_t end = stream_get_getp (s) + caphdr->length;

  SET_FLAG (peer->cap, PEER_CAP_RESTART_RCV);
  restart_flag_time = stream_getw(s);
  if (CHECK_FLAG (restart_flag_time, RESTART_R_BIT))
    restart_bit = 1;
  UNSET_FLAG (restart_flag_time, 0xF000);
  peer->v_gr_restart = restart_flag_time;

  if (BGP_DEBUG (normal, NORMAL))
    {
      zlog_debug ("%s OPEN has Graceful Restart capability", peer->host);
      zlog_debug ("%s Peer has%srestarted. Restart Time : %d",
                  peer->host, restart_bit ? " " : " not ",
                  peer->v_gr_restart);
    }

  while (stream_get_getp (s) + 4 < end)
    {
      afi_t afi = stream_getw (s);
      safi_t safi = stream_getc (s);
      u_char flag = stream_getc (s);
      
      if (!bgp_afi_safi_valid_indices (afi, &safi))
        {
          if (BGP_DEBUG (normal, NORMAL))
            zlog_debug ("%s Addr-family %d/%d(afi/safi) not supported."
                        " Ignore the Graceful Restart capability",
                        peer->host, afi, safi);
        }
      else if (!peer->afc[afi][safi])
        {
          if (BGP_DEBUG (normal, NORMAL))
            zlog_debug ("%s Addr-family %d/%d(afi/safi) not enabled."
                        " Ignore the Graceful Restart capability",
                        peer->host, afi, safi);
        }
      else
        {
          if (BGP_DEBUG (normal, NORMAL))
            zlog_debug ("%s Address family %s is%spreserved", peer->host,
                        afi_safi_print (afi, safi),
                        CHECK_FLAG (peer->af_cap[afi][safi],
                                    PEER_CAP_RESTART_AF_PRESERVE_RCV)
                        ? " " : " not ");

          SET_FLAG (peer->af_cap[afi][safi], PEER_CAP_RESTART_AF_RCV);
          if (CHECK_FLAG (flag, RESTART_F_BIT))
            SET_FLAG (peer->af_cap[afi][safi], PEER_CAP_RESTART_AF_PRESERVE_RCV);
          
        }
    }
  return 0;
}