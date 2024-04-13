parse_cc(Node** np, PToken* tok, UChar** src, UChar* end, ScanEnv* env)
{
  int r, neg, len, fetched, and_start;
  OnigCodePoint in_code, curr_code;
  UChar *p;
  Node* node;
  CClassNode *cc, *prev_cc;
  CClassNode work_cc;
  int curr_raw, in_raw;
  CSTATE state;
  CVAL in_type;
  CVAL curr_type;

  *np = NULL_NODE;
  INC_PARSE_DEPTH(env->parse_depth);

  prev_cc = (CClassNode* )NULL;
  r = fetch_token_in_cc(tok, src, end, env);
  if (r == TK_CHAR && tok->u.code == (OnigCodePoint )'^' && tok->escaped == 0) {
    neg = 1;
    r = fetch_token_in_cc(tok, src, end, env);
  }
  else {
    neg = 0;
  }

  if (r < 0) return r;
  if (r == TK_CC_CLOSE) {
    if (! code_exist_check((OnigCodePoint )']',
                           *src, env->pattern_end, 1, env))
      return ONIGERR_EMPTY_CHAR_CLASS;

    CC_ESC_WARN(env, (UChar* )"]");
    r = tok->type = TK_CHAR;  /* allow []...] */
  }

  *np = node = node_new_cclass();
  CHECK_NULL_RETURN_MEMERR(node);
  cc = CCLASS_(node);

  and_start = 0;
  state = CS_START;
  curr_type = CV_UNDEF;

  p = *src;
  while (r != TK_CC_CLOSE) {
    fetched = 0;
    switch (r) {
    case TK_CHAR:
    any_char_in:
      len = ONIGENC_CODE_TO_MBCLEN(env->enc, tok->u.code);
      if (len < 0) {
        r = len;
        goto err;
      }
      in_type = (len == 1) ? CV_SB : CV_MB;
      in_code = tok->u.code;
      in_raw = 0;
      goto val_entry2;
      break;

    case TK_CRUDE_BYTE:
      /* tok->base != 0 : octal or hexadec. */
      if (! ONIGENC_IS_SINGLEBYTE(env->enc) && tok->base != 0) {
        int i, j;
        UChar buf[ONIGENC_CODE_TO_MBC_MAXLEN];
        UChar* bufe = buf + ONIGENC_CODE_TO_MBC_MAXLEN;
        UChar* psave = p;
        int base = tok->base;

        buf[0] = tok->u.byte;
        for (i = 1; i < ONIGENC_MBC_MAXLEN(env->enc); i++) {
          r = fetch_token_in_cc(tok, &p, end, env);
          if (r < 0) goto err;
          if (r != TK_CRUDE_BYTE || tok->base != base) {
            fetched = 1;
            break;
          }
          buf[i] = tok->u.byte;
        }

        if (i < ONIGENC_MBC_MINLEN(env->enc)) {
          r = ONIGERR_TOO_SHORT_MULTI_BYTE_STRING;
          goto err;
        }

        /* clear buf tail */
        for (j = i; j < ONIGENC_CODE_TO_MBC_MAXLEN; j++) buf[j] = '\0';

        len = enclen(env->enc, buf);
        if (i < len) {
          r = ONIGERR_TOO_SHORT_MULTI_BYTE_STRING;
          goto err;
        }
        else if (i > len) { /* fetch back */
          p = psave;
          for (i = 1; i < len; i++) {
            r = fetch_token_in_cc(tok, &p, end, env);
          }
          fetched = 0;
        }

        if (i == 1) {
          in_code = (OnigCodePoint )buf[0];
          goto crude_single;
        }
        else {
          in_code = ONIGENC_MBC_TO_CODE(env->enc, buf, bufe);
          in_type = CV_MB;
        }
      }
      else {
        in_code = (OnigCodePoint )tok->u.byte;
      crude_single:
        in_type = CV_SB;
      }
      in_raw = 1;
      goto val_entry2;
      break;

    case TK_CODE_POINT:
      in_code = tok->u.code;
      in_raw  = 1;
    val_entry:
      len = ONIGENC_CODE_TO_MBCLEN(env->enc, in_code);
      if (len < 0) {
        if (state != CS_RANGE ||
            ! IS_SYNTAX_BV(env->syntax,
                           ONIG_SYN_ALLOW_INVALID_CODE_END_OF_RANGE_IN_CC) ||
            in_code < 0x100 || ONIGENC_MBC_MAXLEN(env->enc) == 1) {
          r = len;
          goto err;
        }
      }
      in_type = (len == 1 ? CV_SB : CV_MB);
    val_entry2:
      r = cc_char_next(cc, &curr_code, in_code, &curr_raw, in_raw, in_type,
                       &curr_type, &state, env);
      if (r != 0) goto err;
      break;

    case TK_CC_POSIX_BRACKET_OPEN:
      r = parse_posix_bracket(cc, &p, end, env);
      if (r < 0) goto err;
      if (r == 1) {  /* is not POSIX bracket */
        CC_ESC_WARN(env, (UChar* )"[");
        p = tok->backp;
        in_code = tok->u.code;
        in_raw = 0;
        goto val_entry;
      }
      goto next_cprop;
      break;

    case TK_CHAR_TYPE:
      r = add_ctype_to_cc(cc, tok->u.prop.ctype, tok->u.prop.not, env);
      if (r != 0) goto err;

    next_cprop:
      r = cc_cprop_next(cc, &curr_code, &curr_type, &state, env);
      if (r != 0) goto err;
      break;

    case TK_CHAR_PROPERTY:
      {
        int ctype = fetch_char_property_to_ctype(&p, end, env);
        if (ctype < 0) {
          r = ctype;
          goto err;
        }
        r = add_ctype_to_cc(cc, ctype, tok->u.prop.not, env);
        if (r != 0) goto err;
        goto next_cprop;
      }
      break;

    case TK_CC_RANGE:
      if (state == CS_VALUE) {
        r = fetch_token_in_cc(tok, &p, end, env);
        if (r < 0) goto err;

        fetched = 1;
        if (r == TK_CC_CLOSE) { /* allow [x-] */
        range_end_val:
          in_code = (OnigCodePoint )'-';
          in_raw = 0;
          goto val_entry;
        }
        else if (r == TK_CC_AND) {
          CC_ESC_WARN(env, (UChar* )"-");
          goto range_end_val;
        }

        if (curr_type == CV_CPROP) {
          r = ONIGERR_UNMATCHED_RANGE_SPECIFIER_IN_CHAR_CLASS;
          goto err;
        }

        state = CS_RANGE;
      }
      else if (state == CS_START) {
        /* [-xa] is allowed */
        in_code = tok->u.code;
        in_raw = 0;

        r = fetch_token_in_cc(tok, &p, end, env);
        if (r < 0) goto err;

        fetched = 1;
        /* [--x] or [a&&-x] is warned. */
        if (r == TK_CC_RANGE || and_start != 0)
          CC_ESC_WARN(env, (UChar* )"-");

        goto val_entry;
      }
      else if (state == CS_RANGE) {
        CC_ESC_WARN(env, (UChar* )"-");
        goto any_char_in;  /* [!--] is allowed */
      }
      else { /* CS_COMPLETE */
        r = fetch_token_in_cc(tok, &p, end, env);
        if (r < 0) goto err;

        fetched = 1;
        if (r == TK_CC_CLOSE)
          goto range_end_val; /* allow [a-b-] */
        else if (r == TK_CC_AND) {
          CC_ESC_WARN(env, (UChar* )"-");
          goto range_end_val;
        }

        if (IS_SYNTAX_BV(env->syntax, ONIG_SYN_ALLOW_DOUBLE_RANGE_OP_IN_CC)) {
          CC_ESC_WARN(env, (UChar* )"-");
          goto range_end_val;   /* [0-9-a] is allowed as [0-9\-a] */
        }
        r = ONIGERR_UNMATCHED_RANGE_SPECIFIER_IN_CHAR_CLASS;
        goto err;
      }
      break;

    case TK_CC_OPEN_CC: /* [ */
      {
        Node *anode;
        CClassNode* acc;

        if (state == CS_VALUE) {
          r = cc_char_next(cc, &curr_code, 0, &curr_raw, 0, curr_type, &curr_type,
                           &state, env);
          if (r != 0) goto err;
        }
        state = CS_COMPLETE;

        r = parse_cc(&anode, tok, &p, end, env);
        if (r != 0) {
          onig_node_free(anode);
          goto cc_open_err;
        }
        acc = CCLASS_(anode);
        r = or_cclass(cc, acc, env->enc);
        onig_node_free(anode);

      cc_open_err:
        if (r != 0) goto err;
      }
      break;

    case TK_CC_AND: /* && */
      {
        if (state == CS_VALUE) {
          r = cc_char_next(cc, &curr_code, 0, &curr_raw, 0, curr_type, &curr_type,
                           &state, env);
          if (r != 0) goto err;
        }
        /* initialize local variables */
        and_start = 1;
        state = CS_START;

        if (IS_NOT_NULL(prev_cc)) {
          r = and_cclass(prev_cc, cc, env->enc);
          if (r != 0) goto err;
          bbuf_free(cc->mbuf);
        }
        else {
          prev_cc = cc;
          cc = &work_cc;
        }
        initialize_cclass(cc);
      }
      break;

    case TK_EOT:
      r = ONIGERR_PREMATURE_END_OF_CHAR_CLASS;
      goto err;
      break;
    default:
      r = ONIGERR_PARSER_BUG;
      goto err;
      break;
    }

    if (fetched)
      r = tok->type;
    else {
      r = fetch_token_in_cc(tok, &p, end, env);
      if (r < 0) goto err;
    }
  }

  if (state == CS_VALUE) {
    r = cc_char_next(cc, &curr_code, 0, &curr_raw, 0, curr_type, &curr_type,
                     &state, env);
    if (r != 0) goto err;
  }

  if (IS_NOT_NULL(prev_cc)) {
    r = and_cclass(prev_cc, cc, env->enc);
    if (r != 0) goto err;
    bbuf_free(cc->mbuf);
    cc = prev_cc;
  }

  if (neg != 0)
    NCCLASS_SET_NOT(cc);
  else
    NCCLASS_CLEAR_NOT(cc);
  if (IS_NCCLASS_NOT(cc) &&
      IS_SYNTAX_BV(env->syntax, ONIG_SYN_NOT_NEWLINE_IN_NEGATIVE_CC)) {
    int is_empty = (IS_NULL(cc->mbuf) ? 1 : 0);
    if (is_empty != 0)
      BITSET_IS_EMPTY(cc->bs, is_empty);

    if (is_empty == 0) {
#define NEWLINE_CODE    0x0a

      if (ONIGENC_IS_CODE_NEWLINE(env->enc, NEWLINE_CODE)) {
        if (ONIGENC_CODE_TO_MBCLEN(env->enc, NEWLINE_CODE) == 1)
          BITSET_SET_BIT(cc->bs, NEWLINE_CODE);
        else
          add_code_range(&(cc->mbuf), env, NEWLINE_CODE, NEWLINE_CODE);
      }
    }
  }
  *src = p;
  DEC_PARSE_DEPTH(env->parse_depth);
  return 0;

 err:
  if (cc != CCLASS_(*np))
    bbuf_free(cc->mbuf);
  return r;
}