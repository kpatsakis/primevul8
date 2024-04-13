extract_option(uschar **name, uschar **value)
{
uschar *n;
uschar *v = smtp_cmd_data + Ustrlen(smtp_cmd_data) - 1;
while (isspace(*v)) v--;
v[1] = 0;
while (v > smtp_cmd_data && *v != '=' && !isspace(*v))
  {
  /* Take care to not stop at a space embedded in a quoted local-part */

  if (*v == '"') do v--; while (*v != '"' && v > smtp_cmd_data+1);
  v--;
  }

n = v;
if (*v == '=')
  {
  while(isalpha(n[-1])) n--;
  /* RFC says SP, but TAB seen in wild and other major MTAs accept it */
  if (!isspace(n[-1])) return FALSE;
  n[-1] = 0;
  }
else
  {
  n++;
  if (v == smtp_cmd_data) return FALSE;
  }
*v++ = 0;
*name = n;
*value = v;
return TRUE;
}