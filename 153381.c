fetch_name_with_level(OnigCodePoint start_code, UChar** src, UChar* end,
                      UChar** rname_end, ScanEnv* env,
                      int* rback_num, int* rlevel, enum REF_NUM* num_type)
{
  int r, sign, exist_level;
  int digit_count;
  OnigCodePoint end_code;
  OnigCodePoint c = 0;
  OnigEncoding enc = env->enc;
  UChar *name_end;
  UChar *pnum_head;
  UChar *p = *src;
  PFETCH_READY;

  *rback_num = 0;
  exist_level = 0;
  *num_type = IS_NOT_NUM;
  sign = 1;
  pnum_head = *src;

  end_code = get_name_end_code_point(start_code);

  digit_count = 0;
  name_end = end;
  r = 0;
  if (PEND) {
    return ONIGERR_EMPTY_GROUP_NAME;
  }
  else {
    PFETCH(c);
    if (c == end_code)
      return ONIGERR_EMPTY_GROUP_NAME;

    if (IS_CODE_DIGIT_ASCII(enc, c)) {
      *num_type = IS_ABS_NUM;
      digit_count++;
    }
    else if (c == '-') {
      *num_type = IS_REL_NUM;
      sign = -1;
      pnum_head = p;
    }
    else if (c == '+') {
      *num_type = IS_REL_NUM;
      sign = 1;
      pnum_head = p;
    }
    else if (!ONIGENC_IS_CODE_WORD(enc, c)) {
      r = ONIGERR_INVALID_CHAR_IN_GROUP_NAME;
    }
  }

  while (!PEND) {
    name_end = p;
    PFETCH(c);
    if (c == end_code || c == ')' || c == '+' || c == '-') {
      if (*num_type != IS_NOT_NUM && digit_count == 0)
        r = ONIGERR_INVALID_GROUP_NAME;
      break;
    }

    if (*num_type != IS_NOT_NUM) {
      if (IS_CODE_DIGIT_ASCII(enc, c)) {
        digit_count++;
      }
      else {
        r = ONIGERR_INVALID_GROUP_NAME;
        *num_type = IS_NOT_NUM;
      }
    }
    else if (!ONIGENC_IS_CODE_WORD(enc, c)) {
      r = ONIGERR_INVALID_CHAR_IN_GROUP_NAME;
    }
  }

  if (r == 0 && c != end_code) {
    if (c == '+' || c == '-') {
      int level;
      int flag = (c == '-' ? -1 : 1);

      if (PEND) {
        r = ONIGERR_INVALID_CHAR_IN_GROUP_NAME;
        goto end;
      }
      PFETCH(c);
      if (! IS_CODE_DIGIT_ASCII(enc, c)) goto err;
      PUNFETCH;
      level = scan_number(&p, end, enc);
      if (level < 0) return ONIGERR_TOO_BIG_NUMBER;
      *rlevel = (level * flag);
      exist_level = 1;

      if (!PEND) {
        PFETCH(c);
        if (c == end_code)
          goto end;
      }
    }

  err:
    name_end = end;
  err2:
    r = ONIGERR_INVALID_GROUP_NAME;
  }

 end:
  if (r == 0) {
    if (*num_type != IS_NOT_NUM) {
      *rback_num = scan_number(&pnum_head, name_end, enc);
      if (*rback_num < 0) return ONIGERR_TOO_BIG_NUMBER;
      else if (*rback_num == 0) {
        if (*num_type == IS_REL_NUM)
          goto err2;
      }

      *rback_num *= sign;
    }

    *rname_end = name_end;
    *src = p;
    return (exist_level ? 1 : 0);
  }
  else {
    onig_scan_env_set_error_string(env, r, *src, name_end);
    return r;
  }
}