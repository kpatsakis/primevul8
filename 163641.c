testcase_rclass2str(Id rclass)
{
  int i;
  for (i = 0; rclass2str[i].str; i++)
    if (rclass == rclass2str[i].rclass)
      return rclass2str[i].str;
  return "unknown";
}