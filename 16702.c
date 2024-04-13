gpg_ctx_set_istream (struct _GpgCtx *gpg,
                     CamelStream *istream)
{
	g_object_ref (istream);
	if (gpg->istream)
		g_object_unref (gpg->istream);
	gpg->istream = istream;
}