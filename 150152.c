add_state_check_num(regex_t* reg, int num)
{
  StateCheckNumType n = (StateCheckNumType )num;

  BBUF_ADD(reg, &n, SIZE_STATE_CHECK_NUM);
  return 0;
}