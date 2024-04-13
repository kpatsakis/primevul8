static inline void req_fail_link_node(struct io_kiocb *req, int res)
{
	req_set_fail(req);
	req->result = res;
}