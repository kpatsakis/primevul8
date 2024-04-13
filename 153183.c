utf8_type_find (GstTypeFind * tf, gpointer unused)
{
  GstTypeFindProbability start_prob, mid_prob;
  guint64 length;

  /* leave xml to the xml typefinders */
  if (xml_check_first_element (tf, "", 0, TRUE))
    return;

  /* leave sdp to the sdp typefinders */
  if (sdp_check_header (tf))
    return;

  /* check beginning of stream */
  if (!utf8_type_find_have_valid_utf8_at_offset (tf, 0, &start_prob))
    return;

  GST_LOG ("start is plain text with probability of %u", start_prob);

  /* POSSIBLE is the highest probability we ever return if we can't
   * probe into the middle of the file and don't know its length */

  length = gst_type_find_get_length (tf);
  if (length == 0 || length == (guint64) - 1) {
    gst_type_find_suggest (tf, MIN (start_prob, GST_TYPE_FIND_POSSIBLE),
        UTF8_CAPS);
    return;
  }

  if (length < 64 * 1024) {
    gst_type_find_suggest (tf, start_prob, UTF8_CAPS);
    return;
  }

  /* check middle of stream */
  if (!utf8_type_find_have_valid_utf8_at_offset (tf, length / 2, &mid_prob))
    return;

  GST_LOG ("middle is plain text with probability of %u", mid_prob);
  gst_type_find_suggest (tf, (start_prob + mid_prob) / 2, UTF8_CAPS);
}