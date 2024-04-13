form_auth_data_free (EphyEmbedSingleFormAuthData *data)
{
  g_free (data->form_username);
  g_free (data->form_password);
  g_free (data->username);

  g_slice_free (EphyEmbedSingleFormAuthData, data);
}