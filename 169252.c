bgp_attr_extra_free (struct attr *attr)
{
  if (attr->extra)
    {
      XFREE (MTYPE_ATTR_EXTRA, attr->extra);
      attr->extra = NULL;
    }
}