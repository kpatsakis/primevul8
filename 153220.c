qt_type_find (GstTypeFind * tf, gpointer unused)
{
  const guint8 *data;
  guint tip = 0;
  guint64 offset = 0;
  guint64 size;
  const gchar *variant = NULL;

  while ((data = gst_type_find_peek (tf, offset, 12)) != NULL) {
    guint64 new_offset;

    if (STRNCMP (&data[4], "ftypqt  ", 8) == 0) {
      tip = GST_TYPE_FIND_MAXIMUM;
      break;
    }

    if (STRNCMP (&data[4], "ftypisom", 8) == 0 ||
        STRNCMP (&data[4], "ftypavc1", 8) == 0 ||
        STRNCMP (&data[4], "ftypmp42", 8) == 0) {
      tip = GST_TYPE_FIND_MAXIMUM;
      variant = "iso";
      break;
    }

    if (STRNCMP (&data[4], "ftypisml", 8) == 0 ||
        STRNCMP (&data[4], "ftypavc3", 8) == 0) {
      tip = GST_TYPE_FIND_MAXIMUM;
      variant = "iso-fragmented";
      break;
    }

    if (STRNCMP (&data[4], "ftypccff", 8) == 0) {
      tip = GST_TYPE_FIND_MAXIMUM;
      variant = "ccff";
      break;
    }

    /* box/atom types that are in common with ISO base media file format */
    if (STRNCMP (&data[4], "moov", 4) == 0 ||
        STRNCMP (&data[4], "mdat", 4) == 0 ||
        STRNCMP (&data[4], "ftyp", 4) == 0 ||
        STRNCMP (&data[4], "free", 4) == 0 ||
        STRNCMP (&data[4], "uuid", 4) == 0 ||
        STRNCMP (&data[4], "moof", 4) == 0 ||
        STRNCMP (&data[4], "skip", 4) == 0) {
      if (tip == 0) {
        tip = GST_TYPE_FIND_LIKELY;
      } else {
        tip = GST_TYPE_FIND_NEARLY_CERTAIN;
      }
    }
    /* other box/atom types, apparently quicktime specific */
    else if (STRNCMP (&data[4], "pnot", 4) == 0 ||
        STRNCMP (&data[4], "PICT", 4) == 0 ||
        STRNCMP (&data[4], "wide", 4) == 0 ||
        STRNCMP (&data[4], "prfl", 4) == 0) {
      tip = GST_TYPE_FIND_MAXIMUM;
      break;
    } else {
      tip = 0;
      break;
    }

    size = GST_READ_UINT32_BE (data);
    /* check compatible brands rather than ever expaning major brands above */
    if ((STRNCMP (&data[4], "ftyp", 4) == 0) && (size >= 16)) {
      new_offset = offset + 12;
      while (new_offset + 4 <= offset + size) {
        data = gst_type_find_peek (tf, new_offset, 4);
        if (data == NULL)
          goto done;
        if (STRNCMP (&data[4], "isom", 4) == 0 ||
            STRNCMP (&data[4], "dash", 4) == 0 ||
            STRNCMP (&data[4], "avc1", 4) == 0 ||
            STRNCMP (&data[4], "avc3", 4) == 0 ||
            STRNCMP (&data[4], "mp41", 4) == 0 ||
            STRNCMP (&data[4], "mp42", 4) == 0) {
          tip = GST_TYPE_FIND_MAXIMUM;
          variant = "iso";
          goto done;
        }
        new_offset += 4;
      }
    }
    if (size == 1) {
      const guint8 *sizedata;

      sizedata = gst_type_find_peek (tf, offset + 8, 8);
      if (sizedata == NULL)
        break;

      size = GST_READ_UINT64_BE (sizedata);
    } else {
      if (size < 8)
        break;
    }
    new_offset = offset + size;
    if (new_offset <= offset)
      break;
    offset = new_offset;
  }

done:
  if (tip > 0) {
    if (variant) {
      GstCaps *caps = gst_caps_copy (QT_CAPS);

      gst_caps_set_simple (caps, "variant", G_TYPE_STRING, variant, NULL);
      gst_type_find_suggest (tf, tip, caps);
      gst_caps_unref (caps);
    } else {
      gst_type_find_suggest (tf, tip, QT_CAPS);
    }
  }
};