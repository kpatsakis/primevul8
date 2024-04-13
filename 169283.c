bgp_attr_origin (struct bgp_attr_parser_args *args)
{
  struct peer *const peer = args->peer;
  struct attr *const attr = args->attr;
  const bgp_size_t length = args->length;
  
  /* If any recognized attribute has Attribute Length that conflicts
     with the expected length (based on the attribute type code), then
     the Error Subcode is set to Attribute Length Error.  The Data
     field contains the erroneous attribute (type, length and
     value). */
  if (length != 1)
    {
      zlog (peer->log, LOG_ERR, "Origin attribute length is not one %d",
	    length);
      return bgp_attr_malformed (args,
                                 BGP_NOTIFY_UPDATE_ATTR_LENG_ERR,
                                 args->total);
    }

  /* Fetch origin attribute. */
  attr->origin = stream_getc (BGP_INPUT (peer));

  /* If the ORIGIN attribute has an undefined value, then the Error
     Subcode is set to Invalid Origin Attribute.  The Data field
     contains the unrecognized attribute (type, length and value). */
  if ((attr->origin != BGP_ORIGIN_IGP)
      && (attr->origin != BGP_ORIGIN_EGP)
      && (attr->origin != BGP_ORIGIN_INCOMPLETE))
    {
      zlog (peer->log, LOG_ERR, "Origin attribute value is invalid %d",
	      attr->origin);
      return bgp_attr_malformed (args,
                                 BGP_NOTIFY_UPDATE_INVAL_ORIGIN,
                                 args->total);
    }

  /* Set oring attribute flag. */
  attr->flag |= ATTR_FLAG_BIT (BGP_ATTR_ORIGIN);

  return 0;
}