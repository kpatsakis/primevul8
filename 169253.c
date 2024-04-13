bgp_attr_default_set (struct attr *attr, u_char origin)
{
  memset (attr, 0, sizeof (struct attr));
  bgp_attr_extra_get (attr);
  
  attr->origin = origin;
  attr->flag |= ATTR_FLAG_BIT (BGP_ATTR_ORIGIN);
  attr->aspath = aspath_empty ();
  attr->flag |= ATTR_FLAG_BIT (BGP_ATTR_AS_PATH);
  attr->extra->weight = BGP_ATTR_DEFAULT_WEIGHT;
  attr->flag |= ATTR_FLAG_BIT (BGP_ATTR_NEXT_HOP);
#ifdef HAVE_IPV6
  attr->extra->mp_nexthop_len = IPV6_MAX_BYTELEN;
#endif

  return attr;
}