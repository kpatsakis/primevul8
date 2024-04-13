bgp_attr_malformed (struct bgp_attr_parser_args *args, u_char subcode,
                    bgp_size_t length)
{
  struct peer *const peer = args->peer; 
  const u_int8_t flags = args->flags;
  /* startp and length must be special-cased, as whether or not to
   * send the attribute data with the NOTIFY depends on the error,
   * the caller therefore signals this with the seperate length argument
   */
  u_char *notify_datap = (length > 0 ? args->startp : NULL);
  
  /* Only relax error handling for eBGP peers */
  if (peer_sort (peer) != BGP_PEER_EBGP)
    {
      bgp_notify_send_with_data (peer, BGP_NOTIFY_UPDATE_ERR, subcode,
                                 notify_datap, length);
      return BGP_ATTR_PARSE_ERROR;

    }
  
  /* Adjust the stream getp to the end of the attribute, in case we can
   * still proceed but the caller hasn't read all the attribute.
   */
  stream_set_getp (BGP_INPUT (peer),
                   (args->startp - STREAM_DATA (BGP_INPUT (peer)))
                    + args->total);
  
  switch (args->type) {
    /* where an attribute is relatively inconsequential, e.g. it does not
     * affect route selection, and can be safely ignored, then any such
     * attributes which are malformed should just be ignored and the route
     * processed as normal.
     */
    case BGP_ATTR_AS4_AGGREGATOR:
    case BGP_ATTR_AGGREGATOR:
    case BGP_ATTR_ATOMIC_AGGREGATE:
      return BGP_ATTR_PARSE_PROCEED;
    
    /* Core attributes, particularly ones which may influence route
     * selection, should always cause session resets
     */
    case BGP_ATTR_ORIGIN:
    case BGP_ATTR_AS_PATH:
    case BGP_ATTR_NEXT_HOP:
    case BGP_ATTR_MULTI_EXIT_DISC:
    case BGP_ATTR_LOCAL_PREF:
    case BGP_ATTR_COMMUNITIES:
    case BGP_ATTR_ORIGINATOR_ID:
    case BGP_ATTR_CLUSTER_LIST:
    case BGP_ATTR_MP_REACH_NLRI:
    case BGP_ATTR_MP_UNREACH_NLRI:
    case BGP_ATTR_EXT_COMMUNITIES:
      bgp_notify_send_with_data (peer, BGP_NOTIFY_UPDATE_ERR, subcode,
                                 notify_datap, length);
      return BGP_ATTR_PARSE_ERROR;
  }
  
  /* Partial optional attributes that are malformed should not cause
   * the whole session to be reset. Instead treat it as a withdrawal
   * of the routes, if possible.
   */
  if (CHECK_FLAG (flags, BGP_ATTR_FLAG_TRANS)
      && CHECK_FLAG (flags, BGP_ATTR_FLAG_OPTIONAL)
      && CHECK_FLAG (flags, BGP_ATTR_FLAG_PARTIAL))
    return BGP_ATTR_PARSE_WITHDRAW;
  
  /* default to reset */
  return BGP_ATTR_PARSE_ERROR;
}