form_auth_data_new (const char *form_username,
                    const char *form_password,
                    const char *username)
{
  EphyEmbedSingleFormAuthData *data;

  data = g_slice_new (EphyEmbedSingleFormAuthData);
  data->form_username = g_strdup (form_username);
  data->form_password = g_strdup (form_password);
  data->username = g_strdup (username);

  return data;
}