static inline void io_req_complete_state(struct io_kiocb *req, s32 res,
					 u32 cflags)
{
	req->result = res;
	req->cflags = cflags;
	req->flags |= REQ_F_COMPLETE_INLINE;
}