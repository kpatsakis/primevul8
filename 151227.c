static MagickBooleanType IsSFW(const unsigned char *magick,const size_t length)
{
  if (length < 5)
    return(MagickFalse);
  if (LocaleNCompare((const char *) magick,"SFW94",5) == 0)
    return(MagickTrue);
  return(MagickFalse);
}