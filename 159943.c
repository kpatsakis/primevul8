int LibRaw_buffer_datastream::jpeg_src(void *jpegdata)
{
#if defined(NO_JPEG) || !defined(USE_JPEG)
  return -1;
#else
  j_decompress_ptr cinfo = (j_decompress_ptr)jpegdata;
  jpeg_mem_src(cinfo, (unsigned char *)buf + streampos, streamsize - streampos);
  return 0;
#endif
}