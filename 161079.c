
private int
magiccheck(struct magic_set *ms, struct magic *m)
{
	uint64_t l = m->value.q;
	uint64_t v;
	float fl, fv;
	double dl, dv;
	int matched;
	union VALUETYPE *p = &ms->ms_value;

	switch (m->type) {
	case FILE_BYTE:
		v = p->b;
		break;

	case FILE_SHORT:
	case FILE_BESHORT:
	case FILE_LESHORT:
		v = p->h;
		break;

	case FILE_LONG:
	case FILE_BELONG:
	case FILE_LELONG:
	case FILE_MELONG:
	case FILE_DATE:
	case FILE_BEDATE:
	case FILE_LEDATE:
	case FILE_MEDATE:
	case FILE_LDATE:
	case FILE_BELDATE:
	case FILE_LELDATE:
	case FILE_MELDATE:
		v = p->l;
		break;

	case FILE_QUAD:
	case FILE_LEQUAD:
	case FILE_BEQUAD:
	case FILE_QDATE:
	case FILE_BEQDATE:
	case FILE_LEQDATE:
	case FILE_QLDATE:
	case FILE_BEQLDATE:
	case FILE_LEQLDATE:
	case FILE_QWDATE:
	case FILE_BEQWDATE:
	case FILE_LEQWDATE:
		v = p->q;
		break;

	case FILE_FLOAT:
	case FILE_BEFLOAT:
	case FILE_LEFLOAT:
		fl = m->value.f;
		fv = p->f;
		switch (m->reln) {
		case 'x':
			matched = 1;
			break;

		case '!':
			matched = fv != fl;
			break;

		case '=':
			matched = fv == fl;
			break;

		case '>':
			matched = fv > fl;
			break;

		case '<':
			matched = fv < fl;
			break;

		default:
			matched = 0;
			file_magerror(ms, "cannot happen with float: invalid relation `%c'",
			    m->reln);
			return -1;
		}
		return matched;

	case FILE_DOUBLE:
	case FILE_BEDOUBLE:
	case FILE_LEDOUBLE:
		dl = m->value.d;
		dv = p->d;
		switch (m->reln) {
		case 'x':
			matched = 1;
			break;

		case '!':
			matched = dv != dl;
			break;

		case '=':
			matched = dv == dl;
			break;

		case '>':
			matched = dv > dl;
			break;

		case '<':
			matched = dv < dl;
			break;

		default:
			matched = 0;
			file_magerror(ms, "cannot happen with double: invalid relation `%c'", m->reln);
			return -1;
		}
		return matched;

	case FILE_DEFAULT:
		l = 0;
		v = 0;
		break;

	case FILE_STRING:
	case FILE_PSTRING:
		l = 0;
		v = file_strncmp(m->value.s, p->s, (size_t)m->vallen, m->str_flags);
		break;

	case FILE_BESTRING16:
	case FILE_LESTRING16:
		l = 0;
		v = file_strncmp16(m->value.s, p->s, (size_t)m->vallen, m->str_flags);
		break;

	case FILE_SEARCH: { /* search ms->search.s for the string m->value.s */
		size_t slen;
		size_t idx;

		if (ms->search.s == NULL)
			return 0;

		slen = MIN(m->vallen, sizeof(m->value.s));
		l = 0;
		v = 0;

		for (idx = 0; m->str_range == 0 || idx < m->str_range; idx++) {
			if (slen + idx > ms->search.s_len)
				break;

			v = file_strncmp(m->value.s, ms->search.s + idx, slen, m->str_flags);
			if (v == 0) {	/* found match */
				ms->search.offset += idx;
				break;
			}
		}
		break;
	}
	case FILE_REGEX: {
		zval *pattern;
		int options = 0;
		pcre_cache_entry *pce;
		TSRMLS_FETCH();
		
		MAKE_STD_ZVAL(pattern);
		ZVAL_STRINGL(pattern, (char *)m->value.s, m->vallen, 0);
	
		options |= PCRE_MULTILINE;
		
		if (m->str_flags & STRING_IGNORE_CASE) {
			options |= PCRE_CASELESS;
		}
		
		convert_libmagic_pattern(pattern, options);
		
		l = v = 0;
		if ((pce = pcre_get_compiled_regex_cache(Z_STRVAL_P(pattern), Z_STRLEN_P(pattern) TSRMLS_CC)) == NULL) {
			zval_dtor(pattern);
			FREE_ZVAL(pattern);
			return -1;
		} else {
			/* pce now contains the compiled regex */
			zval *retval;
			zval *subpats;
			char *haystack;

			MAKE_STD_ZVAL(retval);
			ALLOC_INIT_ZVAL(subpats);
			
			/* Cut the search len from haystack, equals to REG_STARTEND */
			haystack = estrndup(ms->search.s, ms->search.s_len);

			/* match v = 0, no match v = 1 */
			php_pcre_match_impl(pce, haystack, ms->search.s_len, retval, subpats, 1, 1, PREG_OFFSET_CAPTURE, 0 TSRMLS_CC);
			/* Free haystack */
			efree(haystack);
			
			if (Z_LVAL_P(retval) < 0) {
				zval_ptr_dtor(&subpats);
				FREE_ZVAL(retval);
				zval_dtor(pattern);
				FREE_ZVAL(pattern);
				return -1;
			} else if ((Z_LVAL_P(retval) > 0) && (Z_TYPE_P(subpats) == IS_ARRAY)) {
				
				/* Need to fetch global match which equals pmatch[0] */
				HashTable *ht = Z_ARRVAL_P(subpats);
				HashPosition outer_pos;
				zval *pattern_match = NULL, *pattern_offset = NULL;
				
				zend_hash_internal_pointer_reset_ex(ht, &outer_pos); 
				
				if (zend_hash_has_more_elements_ex(ht, &outer_pos) == SUCCESS &&
					zend_hash_move_forward_ex(ht, &outer_pos)) {
					
					zval **ppzval;
					
					/* The first element (should be) is the global match 
					   Need to move to the inner array to get the global match */
					
					if (zend_hash_get_current_data_ex(ht, (void**)&ppzval, &outer_pos) != FAILURE) { 
						
						HashTable *inner_ht;
						HashPosition inner_pos;
						zval **match, **offset;
						zval tmpcopy = **ppzval, matchcopy, offsetcopy;
						
						zval_copy_ctor(&tmpcopy); 
						INIT_PZVAL(&tmpcopy);
						
						inner_ht = Z_ARRVAL(tmpcopy);
						
						/* If everything goes according to the master plan
						   tmpcopy now contains two elements:
						   0 = the match
						   1 = starting position of the match */
						zend_hash_internal_pointer_reset_ex(inner_ht, &inner_pos); 
						
						if (zend_hash_has_more_elements_ex(inner_ht, &inner_pos) == SUCCESS &&
							zend_hash_move_forward_ex(inner_ht, &inner_pos)) {
						
							if (zend_hash_get_current_data_ex(inner_ht, (void**)&match, &inner_pos) != FAILURE) { 
									
								matchcopy = **match;
								zval_copy_ctor(&matchcopy);
								INIT_PZVAL(&matchcopy);
								convert_to_string(&matchcopy); 
								
								MAKE_STD_ZVAL(pattern_match);
								Z_STRVAL_P(pattern_match) = (char *)Z_STRVAL(matchcopy);
								Z_STRLEN_P(pattern_match) = Z_STRLEN(matchcopy);
								Z_TYPE_P(pattern_match) = IS_STRING; 

								zval_dtor(&matchcopy);
							}
						}
						
						if (zend_hash_has_more_elements_ex(inner_ht, &inner_pos) == SUCCESS &&
							zend_hash_move_forward_ex(inner_ht, &inner_pos)) {
							
							if (zend_hash_get_current_data_ex(inner_ht, (void**)&offset, &inner_pos) != FAILURE) { 
								
								offsetcopy = **offset;
								zval_copy_ctor(&offsetcopy);
								INIT_PZVAL(&offsetcopy);
								convert_to_long(&offsetcopy); 
								
								MAKE_STD_ZVAL(pattern_offset);
								Z_LVAL_P(pattern_offset) = Z_LVAL(offsetcopy);
								Z_TYPE_P(pattern_offset) = IS_LONG;
								
								zval_dtor(&offsetcopy);
							}
						}
						zval_dtor(&tmpcopy); 	
					}
					
					if ((pattern_match != NULL) && (pattern_offset != NULL)) {
						ms->search.s += (int)Z_LVAL_P(pattern_offset); /* this is where the match starts */
						ms->search.offset += (size_t)Z_LVAL_P(pattern_offset); /* this is where the match starts as size_t */
						ms->search.rm_len = Z_STRLEN_P(pattern_match) /* This is the length of the matched pattern */;
						v = 0;
						
						efree(pattern_match);
						efree(pattern_offset);
						
					} else {
						zval_ptr_dtor(&subpats);
						FREE_ZVAL(retval);
						zval_dtor(pattern);
						FREE_ZVAL(pattern);
						return -1;
					}					
				}

				
			} else {
				v = 1;
			}
			zval_ptr_dtor(&subpats);
			FREE_ZVAL(retval);
		}
		zval_dtor(pattern);
		FREE_ZVAL(pattern);
		break;	
	}
	case FILE_INDIRECT:
	case FILE_USE:
	case FILE_NAME:
		return 1;	 
	default:
		file_magerror(ms, "invalid type %d in magiccheck()", m->type);
		return -1;
	}

	v = file_signextend(ms, m, v);

	switch (m->reln) {
	case 'x':
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void) fprintf(stderr, "%" INT64_T_FORMAT
			    "u == *any* = 1\n", (unsigned long long)v);
		matched = 1;
		break;

	case '!':
		matched = v != l;
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void) fprintf(stderr, "%" INT64_T_FORMAT "u != %"
			    INT64_T_FORMAT "u = %d\n", (unsigned long long)v,
			    (unsigned long long)l, matched);
		break;

	case '=':
		matched = v == l;
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void) fprintf(stderr, "%" INT64_T_FORMAT "u == %"
			    INT64_T_FORMAT "u = %d\n", (unsigned long long)v,
			    (unsigned long long)l, matched);
		break;

	case '>':
		if (m->flag & UNSIGNED) {
			matched = v > l;
			if ((ms->flags & MAGIC_DEBUG) != 0)
				(void) fprintf(stderr, "%" INT64_T_FORMAT
				    "u > %" INT64_T_FORMAT "u = %d\n",
				    (unsigned long long)v,
				    (unsigned long long)l, matched);
		}
		else {
			matched = (int64_t) v > (int64_t) l;
			if ((ms->flags & MAGIC_DEBUG) != 0)
				(void) fprintf(stderr, "%" INT64_T_FORMAT
				    "d > %" INT64_T_FORMAT "d = %d\n",
				    (long long)v, (long long)l, matched);
		}
		break;

	case '<':
		if (m->flag & UNSIGNED) {
			matched = v < l;
			if ((ms->flags & MAGIC_DEBUG) != 0)
				(void) fprintf(stderr, "%" INT64_T_FORMAT
				    "u < %" INT64_T_FORMAT "u = %d\n",
				    (unsigned long long)v,
				    (unsigned long long)l, matched);
		}
		else {
			matched = (int64_t) v < (int64_t) l;
			if ((ms->flags & MAGIC_DEBUG) != 0)
				(void) fprintf(stderr, "%" INT64_T_FORMAT
				    "d < %" INT64_T_FORMAT "d = %d\n",
				     (long long)v, (long long)l, matched);
		}
		break;

	case '&':
		matched = (v & l) == l;
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void) fprintf(stderr, "((%" INT64_T_FORMAT "x & %"
			    INT64_T_FORMAT "x) == %" INT64_T_FORMAT
			    "x) = %d\n", (unsigned long long)v,
			    (unsigned long long)l, (unsigned long long)l,
			    matched);
		break;

	case '^':
		matched = (v & l) != l;
		if ((ms->flags & MAGIC_DEBUG) != 0)
			(void) fprintf(stderr, "((%" INT64_T_FORMAT "x & %"
			    INT64_T_FORMAT "x) != %" INT64_T_FORMAT
			    "x) = %d\n", (unsigned long long)v,
			    (unsigned long long)l, (unsigned long long)l,
			    matched);
		break;

	default:
		matched = 0;
		file_magerror(ms, "cannot happen: invalid relation `%c'",
		    m->reln);
		return -1;
	}
