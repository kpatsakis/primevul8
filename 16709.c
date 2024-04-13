gpg_ctx_set_hash (struct _GpgCtx *gpg,
                  CamelCipherHash hash)
{
	gpg->hash = hash;
}