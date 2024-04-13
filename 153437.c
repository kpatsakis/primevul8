fetch_name(OnigCodePoint start_code, UChar** src, UChar* end,
           UChar** rname_end, ScanEnv* env, int* rback_num,
           enum REF_NUM* num_type, int is_ref)
{
  int r, sign;
  int digit_count;
  OnigCodePoint end_code;
  OnigCodePoint c = 0;
  OnigEncoding enc = env->enc;
  UChar *name_end;
  UChar *pnum_head;
  UChar *p = *src;

  *rback_num = 0;

  end_code = get_name_end_code_point(start_code);

  digit_count = 0;
  name_end = end;
  pnum_head = *src;
  r = 0;
  *num_type = IS_NOT_NUM;
  sign = 1;
  if (PEND) {
    return ONIGERR_EMPTY_GROUP_NAME;
  }
  else {
    PFETCH_S(c);
    if (c == end_code)
      return ONIGERR_EMPTY_GROUP_NAME;

    if (IS_CODE_DIGIT_ASCII(enc, c)) {
      if (is_ref == TRUE)
        *num_type = IS_ABS_NUM;
      else {
        r = ONIGERR_INVALID_GROUP_NAME;
      }
      digit_count++;
    }
    else if (c == '-') {
      if (is_ref == TRUE) {
        *num_type = IS_REL_NUM;
        sign = -1;
        pnum_head = p;
      }
      else {
        r = ONIGERR_INVALID_GROUP_NAME;
      }
    }
    else if (c == '+') {
      if (is_ref == TRUE) {
        *num_type = IS_REL_NUM;
        sign = 1;
        pnum_head = p;
      }
      else {
        r = ONIGERR_INVALID_GROUP_NAME;
      }
    }
    else if (!ONIGENC_IS_CODE_WORD(enc, c)) {
      r = ONIGERR_INVALID_CHAR_IN_GROUP_NAME;
    }
  }

  if (r == 0) {
    while (!PEND) {
      name_end = p;
      PFETCH_S(c);
      if (c == end_code || c == ')') {
        if (*num_type != IS_NOT_NUM && digit_count == 0)
          r = ONIGERR_INVALID_GROUP_NAME;
        break;
      }

      if (*num_type != IS_NOT_NUM) {
        if (IS_CODE_DIGIT_ASCII(enc, c)) {
          digit_count++;
        }
        else {
          if (!ONIGENC_IS_CODE_WORD(enc, c))
            r = ONIGERR_INVALID_CHAR_IN_GROUP_NAME;
          else
            r = ONIGERR_INVALID_GROUP_NAME;

          *num_type = IS_NOT_NUM;
        }
      }
      else {
        if (!ONIGENC_IS_CODE_WORD(enc, c)) {
          r = ONIGERR_INVALID_CHAR_IN_GROUP_NAME;
        }
      }
    }

    if (c != end_code) {
      r = ONIGERR_INVALID_GROUP_NAME;
      goto err;
    }

    if (*num_type != IS_NOT_NUM) {
      *rback_num = scan_number(&pnum_head, name_end, enc);
      if (*rback_num < 0) return ONIGERR_TOO_BIG_NUMBER;
      else if (*rback_num == 0) {
        if (*num_type == IS_REL_NUM) {
          r = ONIGERR_INVALID_GROUP_NAME;
          goto err;
        }
      }

      *rback_num *= sign;
    }

    *rname_end = name_end;
    *src = p;
    return 0;
  }
  else {
    while (!PEND) {
      name_end = p;
      PFETCH_S(c);
      if (c == end_code || c == ')')
        break;
    }
    if (PEND)
      name_end = end;

  err:
    onig_scan_env_set_error_string(env, r, *src, name_end);
    return r;
  }
}