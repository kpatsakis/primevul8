gst_riff_create_iavs_template_caps (void)
{
  static const guint32 tags[] = {
    GST_MAKE_FOURCC ('D', 'V', 'S', 'D')
        /* FILL ME */
  };
  guint i;
  GstCaps *caps, *one;

  caps = gst_caps_new_empty ();
  for (i = 0; i < G_N_ELEMENTS (tags); i++) {
    one = gst_riff_create_iavs_caps (tags[i], NULL, NULL, NULL, NULL, NULL);
    if (one)
      gst_caps_append (caps, one);
  }

  return caps;
}