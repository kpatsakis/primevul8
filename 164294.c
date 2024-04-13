gb18030_code_to_mbclen(OnigCodePoint code)
{
       if ((code & 0xff000000) != 0) return 4;
  else if ((code &   0xff0000) != 0) return ONIGERR_INVALID_CODE_POINT_VALUE;
  else if ((code &     0xff00) != 0) return 2;
  else {
    if (GB18030_MAP[(int )(code & 0xff)] == CM)
      return ONIGERR_INVALID_CODE_POINT_VALUE;

    return 1;
  }
}