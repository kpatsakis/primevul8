int do_manual_authentication(const cfg_t *cfg, const device_t *devices,
                             const unsigned n_devs, pam_handle_t *pamh) {
  u2fs_ctx_t *ctx_arr[n_devs];
  u2fs_auth_res_t *auth_result;
  u2fs_rc s_rc;
  char *response = NULL;
  char prompt[MAX_PROMPT_LEN];
  char *buf;
  int retval = -2;
  unsigned i = 0;

  if (u2fs_global_init(0) != U2FS_OK) {
    if (cfg->debug)
      D(cfg->debug_file, "Unable to initialize libu2f-server");
    return retval;
  }

  for (i = 0; i < n_devs; ++i) {

    if (u2fs_init(ctx_arr + i) != U2FS_OK) {
      if (cfg->debug)
        D(cfg->debug_file, "Unable to initialize libu2f-server");
      return retval;
    }

    if ((s_rc = u2fs_set_origin(ctx_arr[i], cfg->origin)) != U2FS_OK) {
      if (cfg->debug)
        D(cfg->debug_file, "Unable to set origin: %s", u2fs_strerror(s_rc));
      return retval;
    }

    if ((s_rc = u2fs_set_appid(ctx_arr[i], cfg->appid)) != U2FS_OK) {
      if (cfg->debug)
        D(cfg->debug_file, "Unable to set appid: %s", u2fs_strerror(s_rc));
      return retval;
    }

    if (cfg->debug)
      D(cfg->debug_file, "Attempting authentication with device number %d", i + 1);

    if ((s_rc = u2fs_set_keyHandle(ctx_arr[i], devices[i].keyHandle)) !=
        U2FS_OK) {
      if (cfg->debug)
        D(cfg->debug_file, "Unable to set keyHandle: %s", u2fs_strerror(s_rc));
      return retval;
    }

    if ((s_rc = u2fs_set_publicKey(ctx_arr[i], devices[i].publicKey)) !=
        U2FS_OK) {
      if (cfg->debug)
        D(cfg->debug_file, "Unable to set publicKey %s", u2fs_strerror(s_rc));
      return retval;
    }

    if ((s_rc = u2fs_authentication_challenge(ctx_arr[i], &buf)) != U2FS_OK) {
      if (cfg->debug)
        D(cfg->debug_file, "Unable to produce authentication challenge: %s",
           u2fs_strerror(s_rc));
      return retval;
    }

    if (cfg->debug)
      D(cfg->debug_file, "Challenge: %s", buf);

    if (i == 0) {
      snprintf(prompt, sizeof(prompt),
                      "Now please copy-paste the below challenge(s) to "
                      "'u2f-host -aauthenticate -o %s'",
              cfg->origin);
      converse(pamh, PAM_TEXT_INFO, prompt);
    }
    converse(pamh, PAM_TEXT_INFO, buf);
    free(buf);
    buf = NULL;
  }

  converse(pamh, PAM_TEXT_INFO,
           "Now, please enter the response(s) below, one per line.");

  retval = -1;

  for (i = 0; i < n_devs; ++i) {
    snprintf(prompt, sizeof(prompt), "[%d]: ", i);
    response = converse(pamh, PAM_PROMPT_ECHO_ON, prompt);
    converse(pamh, PAM_TEXT_INFO, response);

    s_rc = u2fs_authentication_verify(ctx_arr[i], response, &auth_result);
    u2fs_free_auth_res(auth_result);
    if (s_rc == U2FS_OK) {
      retval = 1;
    }
    free(response);
    if (retval == 1) {
        break;
    }
  }

  for (i = 0; i < n_devs; ++i)
    u2fs_done(ctx_arr[i]);
  u2fs_global_done();

  return retval;
}