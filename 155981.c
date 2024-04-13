authorize_user_token (struct cfg *cfg,
		      const char *username,
		      const char *otp_id)
{
  int retval;

  if (cfg->auth_file)
    {
      /* Administrator had configured the file and specified is name
         as an argument for this module.
       */
      retval = check_user_token (cfg, cfg->auth_file, username, otp_id);
    }
  else
    {
      char *userfile = NULL;

      /* Getting file from user home directory
         ..... i.e. ~/.yubico/authorized_yubikeys
       */
      if (! get_user_cfgfile_path (NULL, "authorized_yubikeys", username, &userfile))
	return 0;

      retval = check_user_token (cfg, userfile, username, otp_id);

      free (userfile);
    }

  return retval;
#undef USERFILE
}