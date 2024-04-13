get_attr_cb (GnomeKeyringResult result,
             GnomeKeyringAttributeList *attributes,
             EphyEmbedSingle *single)
{
  int i = 0;
  GnomeKeyringAttribute *attribute;
  char *server = NULL, *username = NULL;

  if (result != GNOME_KEYRING_RESULT_OK)
    return;

  attribute = (GnomeKeyringAttribute*)attributes->data;
  for (i = 0; i < attributes->len; i++) {
    if (server && username)
      break;

    if (attribute[i].type == GNOME_KEYRING_ATTRIBUTE_TYPE_STRING) {
      if (g_str_equal (attribute[i].name, "server")) {
        server = g_strdup (attribute[i].value.string);
      } else if (g_str_equal (attribute[i].name, "user")) {
        username = g_strdup (attribute[i].value.string);
      }
    }
  }

  if (server && username &&
      g_strstr_len (server, -1, "form%5Fusername") &&
      g_strstr_len (server, -1, "form%5Fpassword")) {
    /* This is a stored login/password from a form, cache the form
     * names locally so we don't need to hit the keyring daemon all
     * the time */
    const char *form_username, *form_password;
    GHashTable *t;
    SoupURI *uri = soup_uri_new (server);
    t = soup_form_decode (uri->query);
    form_username = g_hash_table_lookup (t, FORM_USERNAME_KEY);
    form_password = g_hash_table_lookup (t, FORM_PASSWORD_KEY);
    ephy_embed_single_add_form_auth (single, uri->host, form_username, form_password, username);
    soup_uri_free (uri);
    g_hash_table_destroy (t);
  }

  g_free (server);
  g_free (username);
}