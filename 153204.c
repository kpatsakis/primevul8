ogganx_type_find (GstTypeFind * tf, gpointer private)
{
  const gchar *media_type;
  DataScanCtx c = { 0, NULL, 0 };
  guint ogg_syncs = 0;
  guint hdr_count[OGG_NUM] = { 0, };
  static const struct
  {
    const gchar marker[10];
    guint8 marker_size;
    GstOggStreamType stream_type;
  } markers[] = {
    {
    "\001vorbis", 7, OGG_AUDIO}, {
    "\200theora", 7, OGG_VIDEO}, {
    "fLaC", 4, OGG_AUDIO}, {
    "\177FLAC", 5, OGG_AUDIO}, {
    "Speex", 5, OGG_AUDIO}, {
    "CMML\0\0\0\0", 8, OGG_OTHER}, {
    "PCM     ", 8, OGG_AUDIO}, {
    "Annodex", 7, OGG_ANNODEX}, {
    "fishead", 7, OGG_SKELETON}, {
    "AnxData", 7, OGG_ANNODEX}, {
    "CELT    ", 8, OGG_AUDIO}, {
    "\200kate\0\0\0", 8, OGG_KATE}, {
    "BBCD\0", 5, OGG_VIDEO}, {
    "OVP80\1\1", 7, OGG_VIDEO}, {
    "OpusHead", 8, OGG_AUDIO}, {
    "\001audio\0\0\0", 9, OGG_AUDIO}, {
    "\001video\0\0\0", 9, OGG_VIDEO}, {
    "\001text\0\0\0", 9, OGG_OTHER}
  };

  while (c.offset < 4096 && data_scan_ctx_ensure_data (tf, &c, 64)) {
    guint size, i;

    if (memcmp (c.data, "OggS", 5) != 0)
      break;

    ++ogg_syncs;

    /* check if BOS */
    if (c.data[5] != 0x02)
      break;

    /* headers should only have one segment */
    if (c.data[26] != 1)
      break;

    size = c.data[27];
    if (size < 8)
      break;

    data_scan_ctx_advance (tf, &c, 28);

    if (!data_scan_ctx_ensure_data (tf, &c, MAX (size, 8)))
      break;

    for (i = 0; i < G_N_ELEMENTS (markers); ++i) {
      if (memcmp (c.data, markers[i].marker, markers[i].marker_size) == 0) {
        ++hdr_count[markers[i].stream_type];
        break;
      }
    }

    if (i == G_N_ELEMENTS (markers)) {
      GST_MEMDUMP ("unknown Ogg stream marker", c.data, size);
      ++hdr_count[OGG_OTHER];
    }

    data_scan_ctx_advance (tf, &c, size);
  }

  if (ogg_syncs == 0)
    return;

  /* We don't bother with annodex types. FIXME: what about XSPF? */
  if (hdr_count[OGG_VIDEO] > 0) {
    media_type = "video/ogg";
  } else if (hdr_count[OGG_AUDIO] > 0) {
    media_type = "audio/ogg";
  } else if (hdr_count[OGG_KATE] > 0 && hdr_count[OGG_OTHER] == 0) {
    media_type = "application/kate";
  } else {
    media_type = "application/ogg";
  }

  GST_INFO ("found %s (audio:%u, video:%u, annodex:%u, skeleton:%u, other:%u)",
      media_type, hdr_count[OGG_AUDIO], hdr_count[OGG_VIDEO],
      hdr_count[OGG_ANNODEX], hdr_count[OGG_SKELETON], hdr_count[OGG_OTHER]);

  gst_type_find_suggest_simple (tf, GST_TYPE_FIND_MAXIMUM, media_type, NULL);
}