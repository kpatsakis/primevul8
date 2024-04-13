smtp_cmd_hist(void)
{
int  i;
gstring * list = NULL;
uschar * s;

for (i = smtp_ch_index; i < SMTP_HBUFF_SIZE; i++)
  if (smtp_connection_had[i] != SCH_NONE)
    list = string_append_listele(list, ',', smtp_names[smtp_connection_had[i]]);

for (i = 0; i < smtp_ch_index; i++)
  list = string_append_listele(list, ',', smtp_names[smtp_connection_had[i]]);

s = string_from_gstring(list);
return s ? s : US"";
}