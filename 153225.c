mss_manifest_load_utf16 (gunichar2 * utf16_ne, const guint8 * utf16_data,
    gsize data_size, guint data_endianness)
{
  memcpy (utf16_ne, utf16_data, data_size);
  if (data_endianness != G_BYTE_ORDER) {
    guint i;

    for (i = 0; i < data_size / 2; ++i)
      utf16_ne[i] = GUINT16_SWAP_LE_BE (utf16_ne[i]);
  }
}