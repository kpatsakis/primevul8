parse_bag(Node** np, PToken* tok, int term, UChar** src, UChar* end,
          ScanEnv* env)
{
  int r, num;
  Node *target;
  OnigOptionType option;
  OnigCodePoint c;
  int list_capture;
  OnigEncoding enc = env->enc;

  UChar* p = *src;
  PFETCH_READY;

  *np = NULL;
  if (PEND) return ONIGERR_END_PATTERN_WITH_UNMATCHED_PARENTHESIS;

  option = env->options;
  c = PPEEK;
  if (c == '?' && IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_QMARK_GROUP_EFFECT)) {
    PINC;
    if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;

    PFETCH(c);
    switch (c) {
    case ':':   /* (?:...) grouping only */
    group:
      r = fetch_token(tok, &p, end, env);
      if (r < 0) return r;
      r = parse_alts(np, tok, term, &p, end, env, FALSE);
      if (r < 0) return r;
      *src = p;
      return 1; /* group */
      break;

    case '=':
      *np = onig_node_new_anchor(ANCR_PREC_READ, FALSE);
      break;
    case '!':  /*         preceding read */
      *np = onig_node_new_anchor(ANCR_PREC_READ_NOT, FALSE);
      break;
    case '>':            /* (?>...) stop backtrack */
      *np = node_new_bag(BAG_STOP_BACKTRACK);
      break;

    case '\'':
      if (IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_QMARK_LT_NAMED_GROUP)) {
        goto named_group1;
      }
      else
        return ONIGERR_UNDEFINED_GROUP_OPTION;
      break;

    case '<':   /* look behind (?<=...), (?<!...) */
      if (PEND) return ONIGERR_END_PATTERN_WITH_UNMATCHED_PARENTHESIS;
      PFETCH(c);
      if (c == '=')
        *np = onig_node_new_anchor(ANCR_LOOK_BEHIND, FALSE);
      else if (c == '!')
        *np = onig_node_new_anchor(ANCR_LOOK_BEHIND_NOT, FALSE);
      else {
        if (IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_QMARK_LT_NAMED_GROUP)) {
          UChar *name;
          UChar *name_end;
          enum REF_NUM num_type;

          PUNFETCH;
          c = '<';

        named_group1:
          list_capture = 0;

        named_group2:
          name = p;
          r = fetch_name((OnigCodePoint )c, &p, end, &name_end, env, &num,
                         &num_type, FALSE);
          if (r < 0) return r;

          num = scan_env_add_mem_entry(env);
          if (num < 0) return num;
          if (list_capture != 0 && num >= (int )MEM_STATUS_BITS_NUM)
            return ONIGERR_GROUP_NUMBER_OVER_FOR_CAPTURE_HISTORY;

          r = name_add(env->reg, name, name_end, num, env);
          if (r != 0) return r;
          *np = node_new_memory(1);
          CHECK_NULL_RETURN_MEMERR(*np);
          BAG_(*np)->m.regnum = num;
          if (list_capture != 0)
            MEM_STATUS_ON_SIMPLE(env->cap_history, num);
          env->num_named++;
        }
        else {
          return ONIGERR_UNDEFINED_GROUP_OPTION;
        }
      }
      break;

    case '~':
      if (IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_QMARK_TILDE_ABSENT_GROUP)) {
        Node* absent;
        Node* expr;
        int head_bar;
        int is_range_cutter;

        if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;

        if (PPEEK_IS('|')) { /* (?~|generator|absent) */
          PINC;
          if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;

          head_bar = 1;
          if (PPEEK_IS(')')) { /* (?~|)  : range clear */
            PINC;
            r = make_range_clear(np, env);
            if (r != 0) return r;
            goto end;
          }
        }
        else
          head_bar = 0;

        r = fetch_token(tok, &p, end, env);
        if (r < 0) return r;
        r = parse_alts(&absent, tok, term, &p, end, env, TRUE);
        if (r < 0) {
          onig_node_free(absent);
          return r;
        }

        expr = NULL_NODE;
        is_range_cutter = 0;
        if (head_bar != 0) {
          Node* top = absent;
          if (NODE_TYPE(top) != NODE_ALT || IS_NULL(NODE_CDR(top))) {
            expr = NULL_NODE;
            is_range_cutter = 1;
            /* return ONIGERR_INVALID_ABSENT_GROUP_GENERATOR_PATTERN; */
          }
          else {
            absent = NODE_CAR(top);
            expr   = NODE_CDR(top);
            NODE_CAR(top) = NULL_NODE;
            NODE_CDR(top) = NULL_NODE;
            onig_node_free(top);
            if (IS_NULL(NODE_CDR(expr))) {
              top = expr;
              expr = NODE_CAR(top);
              NODE_CAR(top) = NULL_NODE;
              onig_node_free(top);
            }
          }
        }

        r = make_absent_tree(np, absent, expr, is_range_cutter, env);
        if (r != 0) {
          return r;
        }
        goto end;
      }
      else {
        return ONIGERR_UNDEFINED_GROUP_OPTION;
      }
      break;

#ifdef USE_CALLOUT
    case '{':
      if (! IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_QMARK_BRACE_CALLOUT_CONTENTS))
        return ONIGERR_UNDEFINED_GROUP_OPTION;

      r = parse_callout_of_contents(np, ')', &p, end, env);
      if (r != 0) return r;

      goto end;
      break;
#endif

    case '(':
      /* (?()...) */
      if (IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_QMARK_LPAREN_IF_ELSE)) {
        UChar *prev;
        Node* condition;
        int condition_is_checker;

        if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;
        PFETCH(c);
        if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;

        if (IS_CODE_DIGIT_ASCII(enc, c)
            || c == '-' || c == '+' || c == '<' || c == '\'') {
          UChar* name_end;
          int back_num;
          int exist_level;
          int level;
          enum REF_NUM num_type;
          int is_enclosed;

          is_enclosed = (c == '<' || c == '\'') ? 1 : 0;
          if (! is_enclosed)
            PUNFETCH;
          prev = p;
          exist_level = 0;
#ifdef USE_BACKREF_WITH_LEVEL
          name_end = NULL_UCHARP; /* no need. escape gcc warning. */
          r = fetch_name_with_level(
                    (OnigCodePoint )(is_enclosed != 0 ? c : '('),
                    &p, end, &name_end,
                    env, &back_num, &level, &num_type);
          if (r == 1) exist_level = 1;
#else
          r = fetch_name((OnigCodePoint )(is_enclosed != 0 ? c : '('),
                         &p, end, &name_end, env, &back_num, &num_type, TRUE);
#endif
          if (r < 0) {
            if (is_enclosed == 0) {
              goto any_condition;
            }
            else
              return r;
          }

          condition_is_checker = 1;
          if (num_type != IS_NOT_NUM) {
            if (num_type == IS_REL_NUM) {
              back_num = backref_rel_to_abs(back_num, env);
            }
            if (back_num <= 0)
              return ONIGERR_INVALID_BACKREF;

            if (IS_SYNTAX_BV(env->syntax, ONIG_SYN_STRICT_CHECK_BACKREF)) {
              if (back_num > env->num_mem ||
                  IS_NULL(SCANENV_MEMENV(env)[back_num].mem_node))
                return ONIGERR_INVALID_BACKREF;
            }

            condition = node_new_backref_checker(1, &back_num, FALSE,
#ifdef USE_BACKREF_WITH_LEVEL
                                                 exist_level, level,
#endif
                                                 env);
          }
          else {
            int num;
            int* backs;

            num = name_to_group_numbers(env, prev, name_end, &backs);
            if (num <= 0) {
              return ONIGERR_UNDEFINED_NAME_REFERENCE;
            }
            if (IS_SYNTAX_BV(env->syntax, ONIG_SYN_STRICT_CHECK_BACKREF)) {
              int i;
              for (i = 0; i < num; i++) {
                if (backs[i] > env->num_mem ||
                    IS_NULL(SCANENV_MEMENV(env)[backs[i]].mem_node))
                  return ONIGERR_INVALID_BACKREF;
              }
            }

            condition = node_new_backref_checker(num, backs, TRUE,
#ifdef USE_BACKREF_WITH_LEVEL
                                                 exist_level, level,
#endif
                                                 env);
          }

          if (is_enclosed != 0) {
            if (PEND) goto err_if_else;
            PFETCH(c);
            if (c != ')') goto err_if_else;
          }
        }
#ifdef USE_CALLOUT
        else if (c == '?') {
          if (IS_SYNTAX_OP2(env->syntax,
                            ONIG_SYN_OP2_QMARK_BRACE_CALLOUT_CONTENTS)) {
            if (! PEND && PPEEK_IS('{')) {
              /* condition part is callouts of contents: (?(?{...})THEN|ELSE) */
              condition_is_checker = 0;
              PFETCH(c);
              r = parse_callout_of_contents(&condition, ')', &p, end, env);
              if (r != 0) return r;
              goto end_condition;
            }
          }
          goto any_condition;
        }
        else if (c == '*' &&
                 IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_ASTERISK_CALLOUT_NAME)) {
          condition_is_checker = 0;
          r = parse_callout_of_name(&condition, ')', &p, end, env);
          if (r != 0) return r;
          goto end_condition;
        }
#endif
        else {
        any_condition:
          PUNFETCH;
          condition_is_checker = 0;
          r = fetch_token(tok, &p, end, env);
          if (r < 0) return r;
          r = parse_alts(&condition, tok, term, &p, end, env, FALSE);
          if (r < 0) {
            onig_node_free(condition);
            return r;
          }
        }

#ifdef USE_CALLOUT
      end_condition:
#endif
        CHECK_NULL_RETURN_MEMERR(condition);

        if (PEND) {
        err_if_else:
          onig_node_free(condition);
          return ONIGERR_END_PATTERN_IN_GROUP;
        }

        if (PPEEK_IS(')')) { /* case: empty body: make backref checker */
          if (condition_is_checker == 0) {
            onig_node_free(condition);
            return ONIGERR_INVALID_IF_ELSE_SYNTAX;
          }
          PFETCH(c);
          *np = condition;
        }
        else { /* if-else */
          int then_is_empty;
          Node *Then, *Else;

          Then = 0;
          if (PPEEK_IS('|')) {
            PFETCH(c);
            then_is_empty = 1;
          }
          else
            then_is_empty = 0;

          r = fetch_token(tok, &p, end, env);
          if (r < 0) {
            onig_node_free(condition);
            return r;
          }
          r = parse_alts(&target, tok, term, &p, end, env, TRUE);
          if (r < 0) {
            onig_node_free(condition);
            onig_node_free(target);
            return r;
          }

          if (then_is_empty != 0) {
            Else = target;
          }
          else {
            if (NODE_TYPE(target) == NODE_ALT) {
              Then = NODE_CAR(target);
              if (NODE_CDR(NODE_CDR(target)) == NULL_NODE) {
                Else = NODE_CAR(NODE_CDR(target));
                cons_node_free_alone(NODE_CDR(target));
              }
              else {
                Else = NODE_CDR(target);
              }
              cons_node_free_alone(target);
            }
            else {
              Then = target;
              Else = 0;
            }
          }

          *np = node_new_bag_if_else(condition, Then, Else);
          if (IS_NULL(*np)) {
            onig_node_free(condition);
            onig_node_free(Then);
            onig_node_free(Else);
            return ONIGERR_MEMORY;
          }
        }
        goto end;
      }
      else {
        return ONIGERR_UNDEFINED_GROUP_OPTION;
      }
      break;

#ifdef USE_CAPTURE_HISTORY
    case '@':
      if (IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_ATMARK_CAPTURE_HISTORY)) {
        if (IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_QMARK_LT_NAMED_GROUP)) {
          PFETCH(c);
          if (c == '<' || c == '\'') {
            list_capture = 1;
            goto named_group2; /* (?@<name>...) */
          }
          PUNFETCH;
        }

        *np = node_new_memory(0);
        CHECK_NULL_RETURN_MEMERR(*np);
        num = scan_env_add_mem_entry(env);
        if (num < 0) {
          return num;
        }
        else if (num >= (int )MEM_STATUS_BITS_NUM) {
          return ONIGERR_GROUP_NUMBER_OVER_FOR_CAPTURE_HISTORY;
        }
        BAG_(*np)->m.regnum = num;
        MEM_STATUS_ON_SIMPLE(env->cap_history, num);
      }
      else {
        return ONIGERR_UNDEFINED_GROUP_OPTION;
      }
      break;
#endif

#ifdef USE_POSIXLINE_OPTION
    case 'p':
#endif
    case '-': case 'i': case 'm': case 's': case 'x':
    case 'W': case 'D': case 'S': case 'P':
    case 'y':
      {
        int neg = 0;

        while (1) {
          switch (c) {
          case ':':
          case ')':
            break;

          case '-':  neg = 1; break;
          case 'x':  OPTION_NEGATE(option, ONIG_OPTION_EXTEND,     neg); break;
          case 'i':  OPTION_NEGATE(option, ONIG_OPTION_IGNORECASE, neg); break;
          case 's':
            if (IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_OPTION_PERL)) {
              OPTION_NEGATE(option, ONIG_OPTION_MULTILINE,  neg);
            }
            else
              return ONIGERR_UNDEFINED_GROUP_OPTION;
            break;

          case 'm':
            if (IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_OPTION_PERL)) {
              OPTION_NEGATE(option, ONIG_OPTION_SINGLELINE, (neg == 0 ? TRUE : FALSE));
            }
            else if (IS_SYNTAX_OP2(env->syntax,
                        ONIG_SYN_OP2_OPTION_ONIGURUMA|ONIG_SYN_OP2_OPTION_RUBY)) {
              OPTION_NEGATE(option, ONIG_OPTION_MULTILINE,  neg);
            }
            else
              return ONIGERR_UNDEFINED_GROUP_OPTION;
            break;
#ifdef USE_POSIXLINE_OPTION
          case 'p':
            OPTION_NEGATE(option, ONIG_OPTION_MULTILINE|ONIG_OPTION_SINGLELINE, neg);
            break;
#endif
          case 'W': OPTION_NEGATE(option, ONIG_OPTION_WORD_IS_ASCII, neg); break;
          case 'D': OPTION_NEGATE(option, ONIG_OPTION_DIGIT_IS_ASCII, neg); break;
          case 'S': OPTION_NEGATE(option, ONIG_OPTION_SPACE_IS_ASCII, neg); break;
          case 'P': OPTION_NEGATE(option, ONIG_OPTION_POSIX_IS_ASCII, neg); break;

          case 'y': /* y{g}, y{w} */
            {
              if (! IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_OPTION_ONIGURUMA))
                return ONIGERR_UNDEFINED_GROUP_OPTION;

              if (neg != 0) return ONIGERR_UNDEFINED_GROUP_OPTION;

              if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;
              if (! PPEEK_IS('{')) return ONIGERR_UNDEFINED_GROUP_OPTION;
              PFETCH(c);
              if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;
              PFETCH(c);
              switch (c) {
              case 'g':
                if (! ONIGENC_IS_UNICODE_ENCODING(enc))
                  return ONIGERR_UNDEFINED_GROUP_OPTION;

                OPTION_NEGATE(option, ONIG_OPTION_TEXT_SEGMENT_EXTENDED_GRAPHEME_CLUSTER, FALSE);
                OPTION_NEGATE(option, ONIG_OPTION_TEXT_SEGMENT_WORD, TRUE);
                break;
#ifdef USE_UNICODE_WORD_BREAK
              case 'w':
                if (! ONIGENC_IS_UNICODE_ENCODING(enc))
                  return ONIGERR_UNDEFINED_GROUP_OPTION;

                OPTION_NEGATE(option, ONIG_OPTION_TEXT_SEGMENT_WORD, FALSE);
                OPTION_NEGATE(option, ONIG_OPTION_TEXT_SEGMENT_EXTENDED_GRAPHEME_CLUSTER, TRUE);
                break;
#endif
              default:
                return ONIGERR_UNDEFINED_GROUP_OPTION;
                break;
              }
              if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;
              PFETCH(c);
              if (c != '}')
                return ONIGERR_UNDEFINED_GROUP_OPTION;
              break;
            } /* case 'y' */

          default:
            return ONIGERR_UNDEFINED_GROUP_OPTION;
          }

          if (c == ')') {
            *np = node_new_option(option);
            CHECK_NULL_RETURN_MEMERR(*np);
            *src = p;
            return 2; /* option only */
          }
          else if (c == ':') {
            OnigOptionType prev = env->options;

            env->options = option;
            r = fetch_token(tok, &p, end, env);
            if (r < 0) return r;
            r = parse_alts(&target, tok, term, &p, end, env, FALSE);
            env->options = prev;
            if (r < 0) {
              onig_node_free(target);
              return r;
            }
            *np = node_new_option(option);
            CHECK_NULL_RETURN_MEMERR(*np);
            NODE_BODY(*np) = target;
            *src = p;
            return 0;
          }

          if (PEND) return ONIGERR_END_PATTERN_IN_GROUP;
          PFETCH(c);
        } /* while (1) */
      }
      break;

    default:
      return ONIGERR_UNDEFINED_GROUP_OPTION;
    }
  }
#ifdef USE_CALLOUT
  else if (c == '*' &&
           IS_SYNTAX_OP2(env->syntax, ONIG_SYN_OP2_ASTERISK_CALLOUT_NAME)) {
    PINC;
    r = parse_callout_of_name(np, ')', &p, end, env);
    if (r != 0) return r;

    goto end;
  }
#endif
  else {
    if (ONIG_IS_OPTION_ON(env->options, ONIG_OPTION_DONT_CAPTURE_GROUP))
      goto group;

    *np = node_new_memory(0);
    CHECK_NULL_RETURN_MEMERR(*np);
    num = scan_env_add_mem_entry(env);
    if (num < 0) return num;
    BAG_(*np)->m.regnum = num;
  }

  CHECK_NULL_RETURN_MEMERR(*np);
  r = fetch_token(tok, &p, end, env);
  if (r < 0) return r;
  r = parse_alts(&target, tok, term, &p, end, env, FALSE);
  if (r < 0) {
    onig_node_free(target);
    return r;
  }

  NODE_BODY(*np) = target;

  if (NODE_TYPE(*np) == NODE_BAG) {
    if (BAG_(*np)->type == BAG_MEMORY) {
      /* Don't move this to previous of parse_alts() */
      r = scan_env_set_mem_node(env, BAG_(*np)->m.regnum, *np);
      if (r != 0) return r;
    }
  }

 end:
  *src = p;
  return 0;
}