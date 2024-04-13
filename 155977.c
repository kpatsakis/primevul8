do_challenge_response(pam_handle_t *pamh, struct cfg *cfg, const char *username)
{
  char *userfile = NULL, *tmpfile = NULL;
  FILE *f = NULL;
  unsigned char buf[CR_RESPONSE_SIZE + 16], response_hex[CR_RESPONSE_SIZE * 2 + 1];
  int ret;

  unsigned int flags = 0;
  unsigned int response_len = 0;
  unsigned int expect_bytes = 0;
  YK_KEY *yk = NULL;
  CR_STATE state;

  int len;
  char *errstr = NULL;

  ret = PAM_AUTH_ERR;
  flags |= YK_FLAG_MAYBLOCK;

  if (! init_yubikey(&yk)) {
    D(("Failed initializing YubiKey"));
    goto out;
  }

  if (! check_firmware_version(yk, false, true)) {
    D(("YubiKey does not support Challenge-Response (version 2.2 required)"));
    goto out;
  }


  if (! get_user_challenge_file (yk, cfg->chalresp_path, username, &userfile)) {
    D(("Failed getting user challenge file for user %s", username));
    goto out;
  }

  DBG(("Loading challenge from file %s", userfile));

  /* XXX should drop root privileges before opening file in user's home directory */
  f = fopen(userfile, "r");

  if (! load_chalresp_state(f, &state))
    goto out;

  if (fclose(f) < 0) {
    f = NULL;
    goto out;
  }

  if (! challenge_response(yk, state.slot, state.challenge, state.challenge_len,
			   true, flags, false,
			   buf, sizeof(buf), &response_len)) {
    D(("Challenge-response FAILED"));
    goto out;
  }

  /*
   * Check YubiKey response against the expected response
   */

  yubikey_hex_encode(response_hex, (char *)buf, response_len);

  if (memcmp(buf, state.response, response_len) == 0) {
    ret = PAM_SUCCESS;
  } else {
    D(("Unexpected C/R response : %s", response_hex));
    goto out;
  }

  DBG(("Got the expected response, generating new challenge (%i bytes).", CR_CHALLENGE_SIZE));

  errstr = "Error generating new challenge, please check syslog or contact your system administrator";
  if (generate_random(state.challenge, sizeof(state.challenge))) {
    D(("Failed generating new challenge!"));
    goto out;
  }

  errstr = "Error communicating with Yubikey, please check syslog or contact your system administrator";
  if (! challenge_response(yk, state.slot, state.challenge, CR_CHALLENGE_SIZE,
			   true, flags, false,
			   buf, sizeof(buf), &response_len)) {
    D(("Second challenge-response FAILED"));
    goto out;
  }

  /* the yk_* functions leave 'junk' in errno */
  errno = 0;

  /*
   * Write the challenge and response we will expect the next time to the state file.
   */
  if (response_len > sizeof(state.response)) {
    D(("Got too long response ??? (%i/%i)", response_len, sizeof(state.response)));
    goto out;
  }
  memcpy (state.response, buf, response_len);
  state.response_len = response_len;

  /* Write out the new file */
  tmpfile = malloc(strlen(userfile) + 1 + 4);
  if (! tmpfile)
    goto out;
  strcpy(tmpfile, userfile);
  strcat(tmpfile, ".tmp");

  f = fopen(tmpfile, "w");
  if (! f)
    goto out;

  errstr = "Error updating Yubikey challenge, please check syslog or contact your system administrator";
  if (! write_chalresp_state (f, &state))
    goto out;
  if (fclose(f) < 0) {
    f = NULL;
    goto out;
  }
  f = NULL;
  if (rename(tmpfile, userfile) < 0) {
    goto out;
  }

  DBG(("Challenge-response success!"));
  errstr = NULL;

 out:
  if (yk_errno) {
    if (yk_errno == YK_EUSBERR) {
      syslog(LOG_ERR, "USB error: %s", yk_usb_strerror());
      D(("USB error: %s", yk_usb_strerror()));
    } else {
      syslog(LOG_ERR, "Yubikey core error: %s", yk_strerror(yk_errno));
      D(("Yubikey core error: %s", yk_strerror(yk_errno)));
    }
  }

  if (errstr)
    display_error(pamh, errstr);

  if (errno) {
    syslog(LOG_ERR, "Challenge response failed: %s", strerror(errno));
    D(("Challenge response failed: %s", strerror(errno)));
  }

  if (yk)
    yk_close_key(yk);
  yk_release();

  if (f)
    fclose(f);

  free(userfile);
  free(tmpfile);
  return ret;
}