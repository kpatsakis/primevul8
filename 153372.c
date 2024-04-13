fetch_token_in_cc(PToken* tok, UChar** src, UChar* end, ScanEnv* env)
{
  int r;
  OnigCodePoint code;
  OnigCodePoint c, c2;
  OnigSyntaxType* syn = env->syntax;
  OnigEncoding enc = env->enc;
  UChar* prev;
  UChar* p = *src;
  PFETCH_READY;

  if (PEND) {
    tok->type = TK_EOT;
    return tok->type;
  }

  PFETCH(c);
  tok->type = TK_CHAR;
  tok->base = 0;
  tok->u.code = c;
  tok->escaped = 0;

  if (c == ']') {
    tok->type = TK_CC_CLOSE;
  }
  else if (c == '-') {
    tok->type = TK_CC_RANGE;
  }
  else if (c == MC_ESC(syn)) {
    if (! IS_SYNTAX_BV(syn, ONIG_SYN_BACKSLASH_ESCAPE_IN_CC))
      goto end;

    if (PEND) return ONIGERR_END_PATTERN_AT_ESCAPE;

    PFETCH(c);
    tok->escaped = 1;
    tok->u.code = c;
    switch (c) {
    case 'w':
      tok->type = TK_CHAR_TYPE;
      tok->u.prop.ctype = ONIGENC_CTYPE_WORD;
      tok->u.prop.not   = 0;
      break;
    case 'W':
      tok->type = TK_CHAR_TYPE;
      tok->u.prop.ctype = ONIGENC_CTYPE_WORD;
      tok->u.prop.not   = 1;
      break;
    case 'd':
      tok->type = TK_CHAR_TYPE;
      tok->u.prop.ctype = ONIGENC_CTYPE_DIGIT;
      tok->u.prop.not   = 0;
      break;
    case 'D':
      tok->type = TK_CHAR_TYPE;
      tok->u.prop.ctype = ONIGENC_CTYPE_DIGIT;
      tok->u.prop.not   = 1;
      break;
    case 's':
      tok->type = TK_CHAR_TYPE;
      tok->u.prop.ctype = ONIGENC_CTYPE_SPACE;
      tok->u.prop.not   = 0;
      break;
    case 'S':
      tok->type = TK_CHAR_TYPE;
      tok->u.prop.ctype = ONIGENC_CTYPE_SPACE;
      tok->u.prop.not   = 1;
      break;
    case 'h':
      if (! IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_H_XDIGIT)) break;
      tok->type = TK_CHAR_TYPE;
      tok->u.prop.ctype = ONIGENC_CTYPE_XDIGIT;
      tok->u.prop.not   = 0;
      break;
    case 'H':
      if (! IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_H_XDIGIT)) break;
      tok->type = TK_CHAR_TYPE;
      tok->u.prop.ctype = ONIGENC_CTYPE_XDIGIT;
      tok->u.prop.not   = 1;
      break;

    case 'p':
    case 'P':
      if (PEND) break;

      c2 = PPEEK;
      if (c2 == '{' &&
          IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_P_BRACE_CHAR_PROPERTY)) {
        PINC;
        tok->type = TK_CHAR_PROPERTY;
        tok->u.prop.not = c == 'P';

        if (!PEND && IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_P_BRACE_CIRCUMFLEX_NOT)) {
          PFETCH(c2);
          if (c2 == '^') {
            tok->u.prop.not = tok->u.prop.not == 0;
          }
          else
            PUNFETCH;
        }
      }
      break;

    case 'o':
      if (PEND) break;

      prev = p;
      if (PPEEK_IS('{') && IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_O_BRACE_OCTAL)) {
        PINC;
        r = scan_octal_number(&p, end, 0, 11, enc, &code);
        if (r < 0) return r;
        if (!PEND) {
          c2 = PPEEK;
          if (IS_CODE_DIGIT_ASCII(enc, c2))
            return ONIGERR_TOO_LONG_WIDE_CHAR_VALUE;
        }

        if (p > prev + enclen(enc, prev) && !PEND && (PPEEK_IS('}'))) {
          PINC;
          tok->type   = TK_CODE_POINT;
          tok->base   = 8;
          tok->u.code = code;
        }
        else {
          /* can't read nothing or invalid format */
          p = prev;
        }
      }
      break;

    case 'x':
      if (PEND) break;

      prev = p;
      if (PPEEK_IS('{') && IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_X_BRACE_HEX8)) {
        PINC;
        r = scan_hexadecimal_number(&p, end, 0, 8, enc, &code);
        if (r < 0) return r;
        if (!PEND) {
          c2 = PPEEK;
          if (IS_CODE_XDIGIT_ASCII(enc, c2))
            return ONIGERR_TOO_LONG_WIDE_CHAR_VALUE;
        }

        if (p > prev + enclen(enc, prev) && !PEND && (PPEEK_IS('}'))) {
          PINC;
          tok->type   = TK_CODE_POINT;
          tok->base   = 16;
          tok->u.code = code;
        }
        else {
          /* can't read nothing or invalid format */
          p = prev;
        }
      }
      else if (IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_X_HEX2)) {
        r = scan_hexadecimal_number(&p, end, 0, 2, enc, &code);
        if (r < 0) return r;
        if (p == prev) {  /* can't read nothing. */
          code = 0; /* but, it's not error */
        }
        tok->type = TK_CRUDE_BYTE;
        tok->base = 16;
        tok->u.byte = (UChar )code;
      }
      break;

    case 'u':
      if (PEND) break;

      prev = p;
      if (IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_ESC_U_HEX4)) {
        r = scan_hexadecimal_number(&p, end, 4, 4, enc, &code);
        if (r < 0) return r;
        if (p == prev) {  /* can't read nothing. */
          code = 0; /* but, it's not error */
        }
        tok->type   = TK_CODE_POINT;
        tok->base   = 16;
        tok->u.code = code;
      }
      break;

    case '0':
    case '1': case '2': case '3': case '4': case '5': case '6': case '7':
      if (IS_SYNTAX_OP(syn, ONIG_SYN_OP_ESC_OCTAL3)) {
        PUNFETCH;
        prev = p;
        r = scan_octal_number(&p, end, 0, 3, enc, &code);
        if (r < 0) return r;
        if (code >= 256) return ONIGERR_TOO_BIG_NUMBER;
        if (p == prev) {  /* can't read nothing. */
          code = 0; /* but, it's not error */
        }
        tok->type = TK_CRUDE_BYTE;
        tok->base = 8;
        tok->u.byte = (UChar )code;
      }
      break;

    default:
      PUNFETCH;
      r = fetch_escaped_value(&p, end, env, &c2);
      if (r < 0) return r;
      if (tok->u.code != c2) {
        tok->u.code = c2;
        tok->type   = TK_CODE_POINT;
      }
      break;
    }
  }
  else if (c == '[') {
    if (IS_SYNTAX_OP(syn, ONIG_SYN_OP_POSIX_BRACKET) && (PPEEK_IS(':'))) {
      OnigCodePoint send[] = { (OnigCodePoint )':', (OnigCodePoint )']' };
      tok->backp = p; /* point at '[' is read */
      PINC;
      if (str_exist_check_with_esc(send, 2, p, end,
                                   (OnigCodePoint )']', enc, syn)) {
        tok->type = TK_CC_POSIX_BRACKET_OPEN;
      }
      else {
        PUNFETCH;
        goto cc_in_cc;
      }
    }
    else {
    cc_in_cc:
      if (IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_CCLASS_SET_OP)) {
        tok->type = TK_CC_OPEN_CC;
      }
      else {
        CC_ESC_WARN(env, (UChar* )"[");
      }
    }
  }
  else if (c == '&') {
    if (IS_SYNTAX_OP2(syn, ONIG_SYN_OP2_CCLASS_SET_OP) &&
        !PEND && (PPEEK_IS('&'))) {
      PINC;
      tok->type = TK_CC_AND;
    }
  }

 end:
  *src = p;
  return tok->type;
}