bmp_type_find (GstTypeFind * tf, gpointer unused)
{
  DataScanCtx c = { 0, NULL, 0 };
  guint32 struct_size, w, h, planes, bpp;

  if (G_UNLIKELY (!data_scan_ctx_ensure_data (tf, &c, 54)))
    return;

  if (c.data[0] != 'B' || c.data[1] != 'M')
    return;

  /* skip marker + size */
  data_scan_ctx_advance (tf, &c, 2 + 4);

  /* reserved, must be 0 */
  if (c.data[0] != 0 || c.data[1] != 0 || c.data[2] != 0 || c.data[3] != 0)
    return;

  data_scan_ctx_advance (tf, &c, 2 + 2);

  /* offset to start of image data in bytes (check for sanity) */
  GST_LOG ("offset=%u", GST_READ_UINT32_LE (c.data));
  if (GST_READ_UINT32_LE (c.data) > (10 * 1024 * 1024))
    return;

  struct_size = GST_READ_UINT32_LE (c.data + 4);
  GST_LOG ("struct_size=%u", struct_size);

  data_scan_ctx_advance (tf, &c, 4 + 4);

  if (struct_size == 0x0C) {
    w = GST_READ_UINT16_LE (c.data);
    h = GST_READ_UINT16_LE (c.data + 2);
    planes = GST_READ_UINT16_LE (c.data + 2 + 2);
    bpp = GST_READ_UINT16_LE (c.data + 2 + 2 + 2);
  } else if (struct_size == 40 || struct_size == 64 || struct_size == 108
      || struct_size == 124 || struct_size == 0xF0) {
    w = GST_READ_UINT32_LE (c.data);
    h = GST_READ_UINT32_LE (c.data + 4);
    planes = GST_READ_UINT16_LE (c.data + 4 + 4);
    bpp = GST_READ_UINT16_LE (c.data + 4 + 4 + 2);
  } else {
    return;
  }

  /* image sizes sanity check */
  GST_LOG ("w=%u, h=%u, planes=%u, bpp=%u", w, h, planes, bpp);
  if (w == 0 || w > 0xfffff || h == 0 || h > 0xfffff || planes != 1 ||
      (bpp != 1 && bpp != 4 && bpp != 8 && bpp != 16 && bpp != 24 && bpp != 32))
    return;

  gst_type_find_suggest_simple (tf, GST_TYPE_FIND_MAXIMUM, "image/bmp",
      "width", G_TYPE_INT, w, "height", G_TYPE_INT, h, "bpp", G_TYPE_INT, bpp,
      NULL);
}