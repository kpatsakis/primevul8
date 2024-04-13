directoryStringSubstringsMatch(
	int *matchp,
	slap_mask_t flags,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *value,
	void *assertedValue )
{
	int match = 0;
	SubstringsAssertion *sub = assertedValue;
	struct berval left = *value;
	ber_len_t i;
	int priorspace=0;

	if ( !BER_BVISNULL( &sub->sa_initial ) ) {
		if ( sub->sa_initial.bv_len > left.bv_len ) {
			/* not enough left */
			match = 1;
			goto done;
		}

		match = memcmp( sub->sa_initial.bv_val, left.bv_val,
			sub->sa_initial.bv_len );

		if ( match != 0 ) {
			goto done;
		}

		left.bv_val += sub->sa_initial.bv_len;
		left.bv_len -= sub->sa_initial.bv_len;

		priorspace = ASCII_SPACE(
			sub->sa_initial.bv_val[sub->sa_initial.bv_len] );
	}

	if ( sub->sa_any ) {
		for ( i = 0; !BER_BVISNULL( &sub->sa_any[i] ); i++ ) {
			ber_len_t idx;
			char *p;

			if( priorspace && !BER_BVISEMPTY( &sub->sa_any[i] ) 
				&& ASCII_SPACE( sub->sa_any[i].bv_val[0] ))
			{ 
				/* allow next space to match */
				left.bv_val--;
				left.bv_len++;
			}
			priorspace=0;

retry:
			if ( BER_BVISEMPTY( &sub->sa_any[i] ) ) {
				continue;
			}

			if ( sub->sa_any[i].bv_len > left.bv_len ) {
				/* not enough left */
				match = 1;
				goto done;
			}

			p = memchr( left.bv_val, *sub->sa_any[i].bv_val, left.bv_len );

			if( p == NULL ) {
				match = 1;
				goto done;
			}

			idx = p - left.bv_val;

			if ( idx >= left.bv_len ) {
				/* this shouldn't happen */
				return LDAP_OTHER;
			}

			left.bv_val = p;
			left.bv_len -= idx;

			if ( sub->sa_any[i].bv_len > left.bv_len ) {
				/* not enough left */
				match = 1;
				goto done;
			}

			match = memcmp( left.bv_val,
				sub->sa_any[i].bv_val,
				sub->sa_any[i].bv_len );

			if ( match != 0 ) {
				left.bv_val++;
				left.bv_len--;
				goto retry;
			}

			left.bv_val += sub->sa_any[i].bv_len;
			left.bv_len -= sub->sa_any[i].bv_len;

			priorspace = ASCII_SPACE(
				sub->sa_any[i].bv_val[sub->sa_any[i].bv_len] );
		}
	}

	if ( !BER_BVISNULL( &sub->sa_final ) ) {
		if( priorspace && !BER_BVISEMPTY( &sub->sa_final ) 
			&& ASCII_SPACE( sub->sa_final.bv_val[0] ))
		{ 
			/* allow next space to match */
			left.bv_val--;
			left.bv_len++;
		}

		if ( sub->sa_final.bv_len > left.bv_len ) {
			/* not enough left */
			match = 1;
			goto done;
		}

		match = memcmp( sub->sa_final.bv_val,
			&left.bv_val[left.bv_len - sub->sa_final.bv_len],
			sub->sa_final.bv_len );

		if ( match != 0 ) {
			goto done;
		}
	}

done:
	*matchp = match;
	return LDAP_SUCCESS;
}