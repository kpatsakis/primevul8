gpg_ctx_set_mode (struct _GpgCtx *gpg,
                  enum _GpgCtxMode mode)
{
	gpg->mode = mode;
	gpg->need_passwd = ((gpg->mode == GPG_CTX_MODE_SIGN) || (gpg->mode == GPG_CTX_MODE_DECRYPT));
}