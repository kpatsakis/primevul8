add_abs_addr(regex_t* reg, int addr)
{
  AbsAddrType ra = (AbsAddrType )addr;

  BBUF_ADD(reg, &ra, SIZE_ABSADDR);
  return 0;
}