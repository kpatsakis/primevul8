find_oid( struct berval *oid )
{
	int i;

	for ( i=0; !BER_BVISNULL( &oids[i].oid ); i++ ) {
		if ( oids[i].oid.bv_len != oid->bv_len ) continue;
		if ( !strcmp( oids[i].oid.bv_val, oid->bv_val ))
			return &oids[i];
	}
	return NULL;
}