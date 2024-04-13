onig_setup_builtin_monitors_by_ascii_encoded_name(void* fp /* FILE* */)
{
  int id;
  char* name;
  OnigEncoding enc;
  unsigned int ts[4];
  OnigValue opts[4];

  if (IS_NOT_NULL(fp))
    OutFp = (FILE* )fp;
  else
    OutFp = stdout;

  enc = ONIG_ENCODING_ASCII;

  name = "MON";
  ts[0] = ONIG_TYPE_CHAR;
  opts[0].c = '>';
  BC_B_O(name, monitor, 1, ts, 1, opts);

  return ONIG_NORMAL;
}