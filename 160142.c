double LibRaw::getreal(int type)
{
  union {
    char c[8];
    double d;
  } u, v;
  int i, rev;

  switch (type)
  {
  case 3: // ushort "SHORT" (int16u)
    return (unsigned short)get2();
  case 4: // unsigned "LONG" (int32u)
    return (unsigned int)get4();
  case 5: // (unsigned, unsigned) "RATIONAL" (rational64u)
    u.d = (unsigned int)get4();
    v.d = (unsigned int)get4();
    return u.d / (v.d ? v.d : 1);
  case 8: // short "SSHORT" (int16s)
    return (signed short)get2();
  case 9: // int "SLONG" (int32s)
    return (signed int)get4();
  case 10: // (int, int) "SRATIONAL" (rational64s)
    u.d = (signed int)get4();
    v.d = (signed int)get4();
    return u.d / (v.d ? v.d : 1);
  case 11: // float "FLOAT" (float)
    return int_to_float(get4());
  case 12: // double "DOUBLE" (double)
    rev = 7 * ((order == 0x4949) == (ntohs(0x1234) == 0x1234));
    for (i = 0; i < 8; i++)
      u.c[i ^ rev] = fgetc(ifp);
    return u.d;
  default:
    return fgetc(ifp);
  }
}