void forward_cancel_request(PgSocket *server)
{
	bool res;
	PgSocket *req = first_socket(&server->pool->cancel_req_list);

	Assert(req != NULL && req->state == CL_CANCEL);
	Assert(server->state == SV_LOGIN);

	SEND_CancelRequest(res, server, req->cancel_key);

	change_client_state(req, CL_JUSTFREE);
}