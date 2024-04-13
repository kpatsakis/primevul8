bgp_update_packet (struct peer *peer, afi_t afi, safi_t safi)
{
  struct stream *s;
  struct bgp_adj_out *adj;
  struct bgp_advertise *adv;
  struct stream *packet;
  struct bgp_node *rn = NULL;
  struct bgp_info *binfo = NULL;
  bgp_size_t total_attr_len = 0;
  unsigned long pos;
  char buf[BUFSIZ];

  s = peer->work;
  stream_reset (s);

  adv = FIFO_HEAD (&peer->sync[afi][safi]->update);

  while (adv)
    {
      assert (adv->rn);
      rn = adv->rn;
      adj = adv->adj;
      if (adv->binfo)
        binfo = adv->binfo;

      /* When remaining space can't include NLRI and it's length.  */
      if (STREAM_REMAIN (s) <= BGP_NLRI_LENGTH + PSIZE (rn->p.prefixlen))
	break;

      /* If packet is empty, set attribute. */
      if (stream_empty (s))
	{
	  struct prefix_rd *prd = NULL;
	  u_char *tag = NULL;
	  struct peer *from = NULL;
	  
	  if (rn->prn)
	    prd = (struct prefix_rd *) &rn->prn->p;
          if (binfo && binfo->extra)
            {
              tag = binfo->extra->tag;
              from = binfo->peer;
            }
          
	  bgp_packet_set_marker (s, BGP_MSG_UPDATE);
	  stream_putw (s, 0);		
	  pos = stream_get_endp (s);
	  stream_putw (s, 0);
	  total_attr_len = bgp_packet_attribute (NULL, peer, s, 
	                                         adv->baa->attr,
	                                         &rn->p, afi, safi, 
	                                         from, prd, tag);
	  stream_putw_at (s, pos, total_attr_len);
	}

      if (afi == AFI_IP && safi == SAFI_UNICAST)
	stream_put_prefix (s, &rn->p);
      
      if (BGP_DEBUG (update, UPDATE_OUT))
	zlog (peer->log, LOG_DEBUG, "%s send UPDATE %s/%d",
	      peer->host,
	      inet_ntop (rn->p.family, &(rn->p.u.prefix), buf, BUFSIZ),
	      rn->p.prefixlen);

      /* Synchnorize attribute.  */
      if (adj->attr)
	bgp_attr_unintern (adj->attr);
      else
	peer->scount[afi][safi]++;

      adj->attr = bgp_attr_intern (adv->baa->attr);

      adv = bgp_advertise_clean (peer, adj, afi, safi);

      if (! (afi == AFI_IP && safi == SAFI_UNICAST))
	break;
    }
	 
  if (! stream_empty (s))
    {
      bgp_packet_set_size (s);
      packet = stream_dup (s);
      bgp_packet_add (peer, packet);
      BGP_WRITE_ON (peer->t_write, bgp_write, peer->fd);
      stream_reset (s);
      return packet;
    }
  return NULL;
}