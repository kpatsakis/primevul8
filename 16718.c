gpg_ctx_set_sigfile (struct _GpgCtx *gpg,
                     const gchar *sigfile)
{
	g_free (gpg->sigfile);
	gpg->sigfile = g_strdup (sigfile);
}