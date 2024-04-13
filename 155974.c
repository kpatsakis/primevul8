parse_cfg (int flags, int argc, const char **argv, struct cfg *cfg)
{
  int i;

  memset (cfg, 0, sizeof(struct cfg));
  cfg->client_id = -1;
  cfg->token_id_length = DEFAULT_TOKEN_ID_LEN;
  cfg->mode = CLIENT;

  for (i = 0; i < argc; i++)
    {
      if (strncmp (argv[i], "id=", 3) == 0)
	sscanf (argv[i], "id=%d", &cfg->client_id);
      if (strncmp (argv[i], "key=", 4) == 0)
	cfg->client_key = (char *) argv[i] + 4;
      if (strcmp (argv[i], "debug") == 0)
	cfg->debug = 1;
      if (strcmp (argv[i], "alwaysok") == 0)
	cfg->alwaysok = 1;
      if (strcmp (argv[i], "verbose_otp") == 0)
	cfg->verbose_otp = 1;
      if (strcmp (argv[i], "try_first_pass") == 0)
	cfg->try_first_pass = 1;
      if (strcmp (argv[i], "use_first_pass") == 0)
	cfg->use_first_pass = 1;
      if (strncmp (argv[i], "authfile=", 9) == 0)
	cfg->auth_file = (char *) argv[i] + 9;
      if (strncmp (argv[i], "capath=", 7) == 0)
	cfg->capath = (char *) argv[i] + 7;
      if (strncmp (argv[i], "url=", 4) == 0)
	cfg->url = (char *) argv[i] + 4;
      if (strncmp (argv[i], "ldapserver=", 11) == 0)
	cfg->ldapserver = (char *) argv[i] + 11;
      if (strncmp (argv[i], "ldap_uri=", 9) == 0)
	cfg->ldap_uri = (char *) argv[i] + 9;
      if (strncmp (argv[i], "ldapdn=", 7) == 0)
	cfg->ldapdn = (char *) argv[i] + 7;
      if (strncmp (argv[i], "user_attr=", 10) == 0)
	cfg->user_attr = (char *) argv[i] + 10;
      if (strncmp (argv[i], "yubi_attr=", 10) == 0)
	cfg->yubi_attr = (char *) argv[i] + 10;
      if (strncmp (argv[i], "token_id_length=", 16) == 0)
	sscanf (argv[i], "token_id_length=%d", &cfg->token_id_length);
      if (strcmp (argv[i], "mode=challenge-response") == 0)
	cfg->mode = CHRESP;
      if (strcmp (argv[i], "mode=client") == 0)
	cfg->mode = CLIENT;
      if (strncmp (argv[i], "chalresp_path=", 14) == 0)
	cfg->chalresp_path = (char *) argv[i] + 14;
    }

  if (cfg->debug)
    {
      D (("called."));
      D (("flags %d argc %d", flags, argc));
      for (i = 0; i < argc; i++)
	D (("argv[%d]=%s", i, argv[i]));
      D (("id=%d", cfg->client_id));
      D (("key=%s", cfg->client_key ? cfg->client_key : "(null)"));
      D (("debug=%d", cfg->debug));
      D (("alwaysok=%d", cfg->alwaysok));
      D (("verbose_otp=%d", cfg->verbose_otp));
      D (("try_first_pass=%d", cfg->try_first_pass));
      D (("use_first_pass=%d", cfg->use_first_pass));
      D (("authfile=%s", cfg->auth_file ? cfg->auth_file : "(null)"));
      D (("ldapserver=%s", cfg->ldapserver ? cfg->ldapserver : "(null)"));
      D (("ldap_uri=%s", cfg->ldap_uri ? cfg->ldap_uri : "(null)"));
      D (("ldapdn=%s", cfg->ldapdn ? cfg->ldapdn : "(null)"));
      D (("user_attr=%s", cfg->user_attr ? cfg->user_attr : "(null)"));
      D (("yubi_attr=%s", cfg->yubi_attr ? cfg->yubi_attr : "(null)"));
      D (("url=%s", cfg->url ? cfg->url : "(null)"));
      D (("capath=%s", cfg->capath ? cfg->capath : "(null)"));
      D (("token_id_length=%d", cfg->token_id_length));
      D (("mode=%s", cfg->mode == CLIENT ? "client" : "chresp" ));
      D (("chalresp_path=%d", cfg->chalresp_path));
    }
}