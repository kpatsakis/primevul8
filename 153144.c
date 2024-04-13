qtif_type_find (GstTypeFind * tf, gpointer unused)
{
  const guint8 *data;
  gboolean found_idsc = FALSE;
  gboolean found_idat = FALSE;
  guint64 offset = 0;
  guint rounds = 0;

  while ((data = gst_type_find_peek (tf, offset, 8)) != NULL) {
    guint64 size;

    size = GST_READ_UINT32_BE (data);
    if (size == 1) {
      const guint8 *sizedata;

      sizedata = gst_type_find_peek (tf, offset + 8, 8);
      if (sizedata == NULL)
        break;

      size = GST_READ_UINT64_BE (sizedata);
    }
    if (size < 8)
      break;

    if (STRNCMP (data + 4, "idsc", 4) == 0)
      found_idsc = TRUE;
    if (STRNCMP (data + 4, "idat", 4) == 0)
      found_idat = TRUE;

    if (found_idsc && found_idat) {
      gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, QTIF_CAPS);
      return;
    }

    offset += size;
    if (++rounds > QTIF_MAXROUNDS)
      break;
  }

  if (found_idsc || found_idat) {
    gst_type_find_suggest (tf, GST_TYPE_FIND_LIKELY, QTIF_CAPS);
    return;
  }
};