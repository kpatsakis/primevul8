my_ulonglong2str(pTHX_ my_ulonglong val)
{
  char buf[64];
  char *ptr = buf + sizeof(buf) - 1;

  if (val == 0)
    return newSVpvn("0", 1);

  *ptr = '\0';
  while (val > 0)
  {
    *(--ptr) = ('0' + (val % 10));
    val = val / 10;
  }
  return newSVpvn(ptr, (buf+ sizeof(buf) - 1) - ptr);
}