float LibRaw::_CanonConvertAperture(ushort in)
{
  if ((in == (ushort)0xffe0) || (in == (ushort)0x7fff))
    return 0.0f;
  return LibRaw::libraw_powf64l(2.0, in / 64.0);
}