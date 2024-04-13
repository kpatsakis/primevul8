static int _converse(pam_handle_t *pamh, int nargs,
                     const struct pam_message **message,
                     struct pam_response **response) {
  struct pam_conv *conv;
  int retval;

  retval = pam_get_item(pamh, PAM_CONV, (void *)&conv);

  if (retval != PAM_SUCCESS) {
    return retval;
  }

  return conv->conv(nargs, message, response, conv->appdata_ptr);
}