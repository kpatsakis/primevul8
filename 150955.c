remove_form_auth_data (gpointer key, gpointer value, gpointer user_data)
{
  if (value)
    free_form_auth_data_list ((GSList*)value);
}