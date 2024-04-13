DEFUN (show_bgp_memory, 
       show_bgp_memory_cmd,
       "show bgp memory",
       SHOW_STR
       BGP_STR
       "Global BGP memory statistics\n")
{
  char memstrbuf[MTYPE_MEMSTR_LEN];
  unsigned long count;
  
  /* RIB related usage stats */
  count = mtype_stats_alloc (MTYPE_BGP_NODE);
  vty_out (vty, "%ld RIB nodes, using %s of memory%s", count,
           mtype_memstr (memstrbuf, sizeof (memstrbuf),
                         count * sizeof (struct bgp_node)),
           VTY_NEWLINE);
  
  count = mtype_stats_alloc (MTYPE_BGP_ROUTE);
  vty_out (vty, "%ld BGP routes, using %s of memory%s", count,
           mtype_memstr (memstrbuf, sizeof (memstrbuf),
                         count * sizeof (struct bgp_info)),
           VTY_NEWLINE);
  if ((count = mtype_stats_alloc (MTYPE_BGP_ROUTE_EXTRA)))
    vty_out (vty, "%ld BGP route ancillaries, using %s of memory%s", count,
             mtype_memstr (memstrbuf, sizeof (memstrbuf),
                           count * sizeof (struct bgp_info_extra)),
             VTY_NEWLINE);
  
  if ((count = mtype_stats_alloc (MTYPE_BGP_STATIC)))
    vty_out (vty, "%ld Static routes, using %s of memory%s", count,
             mtype_memstr (memstrbuf, sizeof (memstrbuf),
                         count * sizeof (struct bgp_static)),
             VTY_NEWLINE);
  
  /* Adj-In/Out */
  if ((count = mtype_stats_alloc (MTYPE_BGP_ADJ_IN)))
    vty_out (vty, "%ld Adj-In entries, using %s of memory%s", count,
             mtype_memstr (memstrbuf, sizeof (memstrbuf),
                           count * sizeof (struct bgp_adj_in)),
             VTY_NEWLINE);
  if ((count = mtype_stats_alloc (MTYPE_BGP_ADJ_OUT)))
    vty_out (vty, "%ld Adj-Out entries, using %s of memory%s", count,
             mtype_memstr (memstrbuf, sizeof (memstrbuf),
                           count * sizeof (struct bgp_adj_out)),
             VTY_NEWLINE);
  
  if ((count = mtype_stats_alloc (MTYPE_BGP_NEXTHOP_CACHE)))
    vty_out (vty, "%ld Nexthop cache entries, using %s of memory%s", count,
             mtype_memstr (memstrbuf, sizeof (memstrbuf),
                         count * sizeof (struct bgp_nexthop_cache)),
             VTY_NEWLINE);

  if ((count = mtype_stats_alloc (MTYPE_BGP_DAMP_INFO)))
    vty_out (vty, "%ld Dampening entries, using %s of memory%s", count,
             mtype_memstr (memstrbuf, sizeof (memstrbuf),
                         count * sizeof (struct bgp_damp_info)),
             VTY_NEWLINE);

  /* Attributes */
  count = attr_count();
  vty_out (vty, "%ld BGP attributes, using %s of memory%s", count, 
           mtype_memstr (memstrbuf, sizeof (memstrbuf), 
                         count * sizeof(struct attr)), 
           VTY_NEWLINE);
  if ((count = mtype_stats_alloc (MTYPE_ATTR_EXTRA)))
    vty_out (vty, "%ld BGP extra attributes, using %s of memory%s", count, 
             mtype_memstr (memstrbuf, sizeof (memstrbuf), 
                           count * sizeof(struct attr_extra)), 
             VTY_NEWLINE);
  
  if ((count = attr_unknown_count()))
    vty_out (vty, "%ld unknown attributes%s", count, VTY_NEWLINE);
  
  /* AS_PATH attributes */
  count = aspath_count ();
  vty_out (vty, "%ld BGP AS-PATH entries, using %s of memory%s", count,
           mtype_memstr (memstrbuf, sizeof (memstrbuf),
                         count * sizeof (struct aspath)),
           VTY_NEWLINE);
  
  count = mtype_stats_alloc (MTYPE_AS_SEG);
  vty_out (vty, "%ld BGP AS-PATH segments, using %s of memory%s", count,
           mtype_memstr (memstrbuf, sizeof (memstrbuf),
                         count * sizeof (struct assegment)),
           VTY_NEWLINE);
  
  /* Other attributes */
  if ((count = community_count ()))
    vty_out (vty, "%ld BGP community entries, using %s of memory%s", count,
             mtype_memstr (memstrbuf, sizeof (memstrbuf),
                         count * sizeof (struct community)),
             VTY_NEWLINE);
  if ((count = mtype_stats_alloc (MTYPE_ECOMMUNITY)))
    vty_out (vty, "%ld BGP community entries, using %s of memory%s", count,
             mtype_memstr (memstrbuf, sizeof (memstrbuf),
                         count * sizeof (struct ecommunity)),
             VTY_NEWLINE);
  
  if ((count = mtype_stats_alloc (MTYPE_CLUSTER)))
    vty_out (vty, "%ld Cluster lists, using %s of memory%s", count,
             mtype_memstr (memstrbuf, sizeof (memstrbuf),
                         count * sizeof (struct cluster_list)),
             VTY_NEWLINE);
  
  /* Peer related usage */
  count = mtype_stats_alloc (MTYPE_BGP_PEER);
  vty_out (vty, "%ld peers, using %s of memory%s", count,
           mtype_memstr (memstrbuf, sizeof (memstrbuf),
                         count * sizeof (struct peer)),
           VTY_NEWLINE);
  
  if ((count = mtype_stats_alloc (MTYPE_PEER_GROUP)))
    vty_out (vty, "%ld peer groups, using %s of memory%s", count,
             mtype_memstr (memstrbuf, sizeof (memstrbuf),
                           count * sizeof (struct peer_group)),
             VTY_NEWLINE);
  
  /* Other */
  if ((count = mtype_stats_alloc (MTYPE_HASH)))
    vty_out (vty, "%ld hash tables, using %s of memory%s", count,
             mtype_memstr (memstrbuf, sizeof (memstrbuf),
                           count * sizeof (struct hash)),
             VTY_NEWLINE);
  if ((count = mtype_stats_alloc (MTYPE_HASH_BACKET)))
    vty_out (vty, "%ld hash buckets, using %s of memory%s", count,
             mtype_memstr (memstrbuf, sizeof (memstrbuf),
                           count * sizeof (struct hash_backet)),
             VTY_NEWLINE);
  if ((count = mtype_stats_alloc (MTYPE_BGP_REGEXP)))
    vty_out (vty, "%ld compiled regexes, using %s of memory%s", count,
             mtype_memstr (memstrbuf, sizeof (memstrbuf),
                           count * sizeof (regex_t)),
             VTY_NEWLINE);
  return CMD_SUCCESS;
}