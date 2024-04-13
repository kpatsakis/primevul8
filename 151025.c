setup_subexp_call(Node* node, ScanEnv* env)
{
  int type;
  int r = 0;

  type = NTYPE(node);
  switch (type) {
  case NT_LIST:
    do {
      r = setup_subexp_call(NCAR(node), env);
    } while (r == 0 && IS_NOT_NULL(node = NCDR(node)));
    break;

  case NT_ALT:
    do {
      r = setup_subexp_call(NCAR(node), env);
    } while (r == 0 && IS_NOT_NULL(node = NCDR(node)));
    break;

  case NT_QTFR:
    r = setup_subexp_call(NQTFR(node)->target, env);
    break;
  case NT_ENCLOSE:
    r = setup_subexp_call(NENCLOSE(node)->target, env);
    break;

  case NT_CALL:
    {
      CallNode* cn = NCALL(node);
      Node** nodes = SCANENV_MEM_NODES(env);

      if (cn->group_num != 0) {
	int gnum = cn->group_num;

#ifdef USE_NAMED_GROUP
	if (env->num_named > 0 &&
	    IS_SYNTAX_BV(env->syntax, ONIG_SYN_CAPTURE_ONLY_NAMED_GROUP) &&
	    !ONIG_IS_OPTION_ON(env->option, ONIG_OPTION_CAPTURE_GROUP)) {
	  return ONIGERR_NUMBERED_BACKREF_OR_CALL_NOT_ALLOWED;
	}
#endif
	if (gnum > env->num_mem) {
	  onig_scan_env_set_error_string(env,
		 ONIGERR_UNDEFINED_GROUP_REFERENCE, cn->name, cn->name_end);
	  return ONIGERR_UNDEFINED_GROUP_REFERENCE;
	}

#ifdef USE_NAMED_GROUP
      set_call_attr:
#endif
	cn->target = nodes[cn->group_num];
	if (IS_NULL(cn->target)) {
	  onig_scan_env_set_error_string(env,
		 ONIGERR_UNDEFINED_NAME_REFERENCE, cn->name, cn->name_end);
	  return ONIGERR_UNDEFINED_NAME_REFERENCE;
	}
	SET_ENCLOSE_STATUS(cn->target, NST_CALLED);
	BIT_STATUS_ON_AT(env->bt_mem_start, cn->group_num);
	cn->unset_addr_list = env->unset_addr_list;
      }
#ifdef USE_NAMED_GROUP
      else {
	int *refs;

	int n = onig_name_to_group_numbers(env->reg, cn->name, cn->name_end,
					   &refs);
	if (n <= 0) {
	  onig_scan_env_set_error_string(env,
		 ONIGERR_UNDEFINED_NAME_REFERENCE, cn->name, cn->name_end);
	  return ONIGERR_UNDEFINED_NAME_REFERENCE;
	}
	else if (n > 1) {
	  onig_scan_env_set_error_string(env,
	    ONIGERR_MULTIPLEX_DEFINITION_NAME_CALL, cn->name, cn->name_end);
	  return ONIGERR_MULTIPLEX_DEFINITION_NAME_CALL;
	}
	else {
	  cn->group_num = refs[0];
	  goto set_call_attr;
	}
      }
#endif
    }
    break;

  case NT_ANCHOR:
    {
      AnchorNode* an = NANCHOR(node);

      switch (an->type) {
      case ANCHOR_PREC_READ:
      case ANCHOR_PREC_READ_NOT:
      case ANCHOR_LOOK_BEHIND:
      case ANCHOR_LOOK_BEHIND_NOT:
	r = setup_subexp_call(an->target, env);
	break;
      }
    }
    break;

  default:
    break;
  }

  return r;
}