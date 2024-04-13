gpg_ctx_set_ostream (struct _GpgCtx *gpg,
                     CamelStream *ostream)
{
	g_object_ref (ostream);
	if (gpg->ostream)
		g_object_unref (gpg->ostream);
	gpg->ostream = ostream;
	gpg->seen_eof1 = FALSE;
}