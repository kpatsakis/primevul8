pcx_header_to_buffer (guint8 *buf)
{
  gint i;
  gint buf_offset = 0;

  for (i = 0; pcx_header_buf_xlate[i].size != 0; i++)
    {
      g_memmove (buf + buf_offset, pcx_header_buf_xlate[i].address,
                 pcx_header_buf_xlate[i].size);
      buf_offset += pcx_header_buf_xlate[i].size;
    }
}