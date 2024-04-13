sdp_check_header (GstTypeFind * tf)
{
  const guint8 *data;

  data = gst_type_find_peek (tf, 0, 5);
  if (!data)
    return FALSE;

  /* sdp must start with v=0[\r]\n */
  if (memcmp (data, "v=0", 3))
    return FALSE;

  if (data[3] == '\r' && data[4] == '\n')
    return TRUE;
  if (data[3] == '\n')
    return TRUE;

  return FALSE;
}