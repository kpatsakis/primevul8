gpg_ctx_op_complete (struct _GpgCtx *gpg)
{
	return gpg->complete && gpg->seen_eof1 && gpg->seen_eof2;}