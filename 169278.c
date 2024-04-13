bgp_attr_hash_alloc (void *p)
{
  struct attr * val = (struct attr *) p;
  struct attr *attr;

  attr = XMALLOC (MTYPE_ATTR, sizeof (struct attr));
  *attr = *val;
  if (val->extra)
    {
      attr->extra = bgp_attr_extra_new ();
      *attr->extra = *val->extra;
    }
  attr->refcnt = 0;
  return attr;
}