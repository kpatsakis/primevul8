testcase_reason2str(Id reason)
{
  int i;
  for (i = 0; reason2str[i].str; i++)
    if (reason == reason2str[i].reason)
      return reason2str[i].str;
  return "?";
}