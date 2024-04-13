void reuse_just_freed_objects(void)
{
	struct List *tmp, *item;
	PgSocket *sk;
	bool close_works = true;

	/*
	 * event_del() may fail because of ENOMEM for event handlers
	 * that need only changes sent to kernel on each loop.
	 *
	 * Keep open sbufs in justfree lists until successful.
	 */

	statlist_for_each_safe(item, &justfree_client_list, tmp) {
		sk = container_of(item, PgSocket, head);
		if (sbuf_is_closed(&sk->sbuf))
			change_client_state(sk, CL_FREE);
		else if (close_works)
			close_works = sbuf_close(&sk->sbuf);
	}
	statlist_for_each_safe(item, &justfree_server_list, tmp) {
		sk = container_of(item, PgSocket, head);
		if (sbuf_is_closed(&sk->sbuf))
			change_server_state(sk, SV_FREE);
		else if (close_works)
			close_works = sbuf_close(&sk->sbuf);
	}
}