static void jpegErrorExit_k(j_common_ptr cinfo)
{
  throw LIBRAW_EXCEPTION_DECODE_JPEG;
}