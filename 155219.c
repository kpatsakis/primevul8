tls_ctx_ref( tls_ctx *ctx )
{
	if ( !ctx ) return;

	tls_imp->ti_ctx_ref( ctx );
}