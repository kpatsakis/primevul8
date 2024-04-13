int do_authentication(const cfg_t *cfg, const device_t *devices,
                      const unsigned n_devs, pam_handle_t *pamh) {
  u2fs_ctx_t *ctx;
  u2fs_auth_res_t *auth_result;
  u2fs_rc s_rc;
  u2fh_rc h_rc;
  u2fh_devs *devs = NULL;
  char *response = NULL;
  char *buf;
  int retval = -2;
  int cued = 0;
  unsigned i = 0;
  unsigned max_index = 0;
  unsigned max_index_prev = 0;

  h_rc = u2fh_global_init(cfg->debug ? U2FH_DEBUG : 0);
  if (h_rc != U2FH_OK) {
    D(cfg->debug_file, "Unable to initialize libu2f-host: %s", u2fh_strerror(h_rc));
    return retval;
  }
  h_rc = u2fh_devs_init(&devs);
  if (h_rc != U2FH_OK) {
    D(cfg->debug_file, "Unable to initialize libu2f-host device handles: %s",
       u2fh_strerror(h_rc));
    return retval;
  }

  if ((h_rc = u2fh_devs_discover(devs, &max_index)) != U2FH_OK) {
    if (cfg->debug)
      D(cfg->debug_file, "Unable to discover device(s), %s", u2fh_strerror(h_rc));
    return retval;
  }
  max_index_prev = max_index;

  if (cfg->debug)
    D(cfg->debug_file, "Device max index is %u", max_index);

  s_rc = u2fs_global_init(cfg->debug ? U2FS_DEBUG : 0);
  if (s_rc != U2FS_OK) {
    D(cfg->debug_file, "Unable to initialize libu2f-server: %s", u2fs_strerror(s_rc));
    return retval;
  }
  s_rc = u2fs_init(&ctx);
  if (s_rc != U2FS_OK) {
    D(cfg->debug_file, "Unable to initialize libu2f-server context: %s", u2fs_strerror(s_rc));
    return retval;
  }

  if ((s_rc = u2fs_set_origin(ctx, cfg->origin)) != U2FS_OK) {
    if (cfg->debug)
      D(cfg->debug_file, "Unable to set origin: %s", u2fs_strerror(s_rc));
    return retval;
  }

  if ((s_rc = u2fs_set_appid(ctx, cfg->appid)) != U2FS_OK) {
    if (cfg->debug)
      D(cfg->debug_file, "Unable to set appid: %s", u2fs_strerror(s_rc));
    return retval;
  }

  if (cfg->nodetect && cfg->debug)
    D(cfg->debug_file, "nodetect option specified, suitable key detection will be skipped");

  i = 0;
  while (i < n_devs) {

    retval = -2;

    if (cfg->debug)
      D(cfg->debug_file, "Attempting authentication with device number %d", i + 1);

    if ((s_rc = u2fs_set_keyHandle(ctx, devices[i].keyHandle)) != U2FS_OK) {
      if (cfg->debug)
        D(cfg->debug_file, "Unable to set keyHandle: %s", u2fs_strerror(s_rc));
      return retval;
    }

    if ((s_rc = u2fs_set_publicKey(ctx, devices[i].publicKey)) != U2FS_OK) {
      if (cfg->debug)
        D(cfg->debug_file, "Unable to set publicKey %s", u2fs_strerror(s_rc));
      return retval;
    }

    if ((s_rc = u2fs_authentication_challenge(ctx, &buf)) != U2FS_OK) {
      if (cfg->debug)
        D(cfg->debug_file, "Unable to produce authentication challenge: %s",
           u2fs_strerror(s_rc));
      free(buf);
      buf = NULL;
      return retval;
    }

    if (cfg->debug)
      D(cfg->debug_file, "Challenge: %s", buf);

    if (cfg->nodetect || (h_rc = u2fh_authenticate(devs, buf, cfg->origin, &response, 0)) == U2FH_OK ) {

      if (cfg->manual == 0 && cfg->cue && !cued) {
        cued = 1;
        converse(pamh, PAM_TEXT_INFO, DEFAULT_CUE);
      }

      retval = -1;

      if ((h_rc = u2fh_authenticate(devs, buf, cfg->origin, &response, U2FH_REQUEST_USER_PRESENCE)) ==
          U2FH_OK) {
        if (cfg->debug)
          D(cfg->debug_file, "Response: %s", response);

        s_rc = u2fs_authentication_verify(ctx, response, &auth_result);
        u2fs_free_auth_res(auth_result);
        free(response);
        response = NULL;
        if (s_rc == U2FS_OK) {
          retval = 1;

          free(buf);
          buf = NULL;
          break;
        }
      } else {
        if (cfg->debug)
          D(cfg->debug_file, "Unable to communicate to the device, %s", u2fh_strerror(h_rc));
      }
    } else {
      if (cfg->debug)
        D(cfg->debug_file, "Device for this keyhandle is not present.");
    }
    free(buf);
    buf = NULL;

    i++;

    if (u2fh_devs_discover(devs, &max_index) != U2FH_OK) {
      if (cfg->debug)
        D(cfg->debug_file, "Unable to discover devices");
      return retval;
    }

    if (max_index > max_index_prev) {
      if (cfg->debug)
        D(cfg->debug_file, "Devices max_index has changed: %u (was %u). Starting over",
           max_index, max_index_prev);
      max_index_prev = max_index;
      i = 0;
    }
  }

  u2fh_devs_done(devs);
  u2fh_global_done();

  u2fs_done(ctx);
  u2fs_global_done();

  return retval;
}