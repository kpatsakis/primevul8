static void jpegErrorExit_d(j_common_ptr cinfo)
{
  throw LIBRAW_EXCEPTION_DECODE_JPEG;
}