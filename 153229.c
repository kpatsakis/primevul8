sdp_type_find (GstTypeFind * tf, gpointer unused)
{
  if (sdp_check_header (tf))
    gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, SDP_CAPS);
}