add_opcode(regex_t* reg, int opcode)
{
  BBUF_ADD1(reg, opcode);
  return 0;
}