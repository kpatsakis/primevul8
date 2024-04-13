set_ref_in_item(
    typval_T	    *tv,
    int		    copyID,
    ht_stack_T	    **ht_stack,
    list_stack_T    **list_stack)
{
    int		abort = FALSE;

    if (tv->v_type == VAR_DICT)
    {
	dict_T	*dd = tv->vval.v_dict;

	if (dd != NULL && dd->dv_copyID != copyID)
	{
	    // Didn't see this dict yet.
	    dd->dv_copyID = copyID;
	    if (ht_stack == NULL)
	    {
		abort = set_ref_in_ht(&dd->dv_hashtab, copyID, list_stack);
	    }
	    else
	    {
		ht_stack_T *newitem = ALLOC_ONE(ht_stack_T);

		if (newitem == NULL)
		    abort = TRUE;
		else
		{
		    newitem->ht = &dd->dv_hashtab;
		    newitem->prev = *ht_stack;
		    *ht_stack = newitem;
		}
	    }
	}
    }
    else if (tv->v_type == VAR_LIST)
    {
	list_T	*ll = tv->vval.v_list;

	if (ll != NULL && ll->lv_copyID != copyID)
	{
	    // Didn't see this list yet.
	    ll->lv_copyID = copyID;
	    if (list_stack == NULL)
	    {
		abort = set_ref_in_list_items(ll, copyID, ht_stack);
	    }
	    else
	    {
		list_stack_T *newitem = ALLOC_ONE(list_stack_T);

		if (newitem == NULL)
		    abort = TRUE;
		else
		{
		    newitem->list = ll;
		    newitem->prev = *list_stack;
		    *list_stack = newitem;
		}
	    }
	}
    }
    else if (tv->v_type == VAR_FUNC)
    {
	abort = set_ref_in_func(tv->vval.v_string, NULL, copyID);
    }
    else if (tv->v_type == VAR_PARTIAL)
    {
	partial_T	*pt = tv->vval.v_partial;
	int		i;

	if (pt != NULL && pt->pt_copyID != copyID)
	{
	    // Didn't see this partial yet.
	    pt->pt_copyID = copyID;

	    abort = set_ref_in_func(pt->pt_name, pt->pt_func, copyID);

	    if (pt->pt_dict != NULL)
	    {
		typval_T dtv;

		dtv.v_type = VAR_DICT;
		dtv.vval.v_dict = pt->pt_dict;
		set_ref_in_item(&dtv, copyID, ht_stack, list_stack);
	    }

	    for (i = 0; i < pt->pt_argc; ++i)
		abort = abort || set_ref_in_item(&pt->pt_argv[i], copyID,
							ht_stack, list_stack);
	    // pt_funcstack is handled in set_ref_in_funcstacks()
	}
    }
#ifdef FEAT_JOB_CHANNEL
    else if (tv->v_type == VAR_JOB)
    {
	job_T	    *job = tv->vval.v_job;
	typval_T    dtv;

	if (job != NULL && job->jv_copyID != copyID)
	{
	    job->jv_copyID = copyID;
	    if (job->jv_channel != NULL)
	    {
		dtv.v_type = VAR_CHANNEL;
		dtv.vval.v_channel = job->jv_channel;
		set_ref_in_item(&dtv, copyID, ht_stack, list_stack);
	    }
	    if (job->jv_exit_cb.cb_partial != NULL)
	    {
		dtv.v_type = VAR_PARTIAL;
		dtv.vval.v_partial = job->jv_exit_cb.cb_partial;
		set_ref_in_item(&dtv, copyID, ht_stack, list_stack);
	    }
	}
    }
    else if (tv->v_type == VAR_CHANNEL)
    {
	channel_T   *ch =tv->vval.v_channel;
	ch_part_T   part;
	typval_T    dtv;
	jsonq_T	    *jq;
	cbq_T	    *cq;

	if (ch != NULL && ch->ch_copyID != copyID)
	{
	    ch->ch_copyID = copyID;
	    for (part = PART_SOCK; part < PART_COUNT; ++part)
	    {
		for (jq = ch->ch_part[part].ch_json_head.jq_next; jq != NULL;
							     jq = jq->jq_next)
		    set_ref_in_item(jq->jq_value, copyID, ht_stack, list_stack);
		for (cq = ch->ch_part[part].ch_cb_head.cq_next; cq != NULL;
							     cq = cq->cq_next)
		    if (cq->cq_callback.cb_partial != NULL)
		    {
			dtv.v_type = VAR_PARTIAL;
			dtv.vval.v_partial = cq->cq_callback.cb_partial;
			set_ref_in_item(&dtv, copyID, ht_stack, list_stack);
		    }
		if (ch->ch_part[part].ch_callback.cb_partial != NULL)
		{
		    dtv.v_type = VAR_PARTIAL;
		    dtv.vval.v_partial =
				      ch->ch_part[part].ch_callback.cb_partial;
		    set_ref_in_item(&dtv, copyID, ht_stack, list_stack);
		}
	    }
	    if (ch->ch_callback.cb_partial != NULL)
	    {
		dtv.v_type = VAR_PARTIAL;
		dtv.vval.v_partial = ch->ch_callback.cb_partial;
		set_ref_in_item(&dtv, copyID, ht_stack, list_stack);
	    }
	    if (ch->ch_close_cb.cb_partial != NULL)
	    {
		dtv.v_type = VAR_PARTIAL;
		dtv.vval.v_partial = ch->ch_close_cb.cb_partial;
		set_ref_in_item(&dtv, copyID, ht_stack, list_stack);
	    }
	}
    }
#endif
    return abort;
}