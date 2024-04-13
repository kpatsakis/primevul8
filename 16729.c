gpg_ctx_set_always_trust (struct _GpgCtx *gpg,
                          gboolean trust)
{
	gpg->always_trust = trust;
}