static void io_req_complete_fail_submit(struct io_kiocb *req)
{
	/*
	 * We don't submit, fail them all, for that replace hardlinks with
	 * normal links. Extra REQ_F_LINK is tolerated.
	 */
	req->flags &= ~REQ_F_HARDLINK;
	req->flags |= REQ_F_LINK;
	io_req_complete_failed(req, req->result);
}