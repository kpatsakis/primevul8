static int io_recvmsg(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_async_msghdr iomsg, *kmsg;
	struct io_sr_msg *sr = &req->sr_msg;
	struct socket *sock;
	struct io_buffer *kbuf;
	unsigned flags;
	int ret, min_ret = 0;
	bool force_nonblock = issue_flags & IO_URING_F_NONBLOCK;

	sock = sock_from_file(req->file);
	if (unlikely(!sock))
		return -ENOTSOCK;

	if (req_has_async_data(req)) {
		kmsg = req->async_data;
	} else {
		ret = io_recvmsg_copy_hdr(req, &iomsg);
		if (ret)
			return ret;
		kmsg = &iomsg;
	}

	if (req->flags & REQ_F_BUFFER_SELECT) {
		kbuf = io_recv_buffer_select(req, issue_flags);
		if (IS_ERR(kbuf))
			return PTR_ERR(kbuf);
		kmsg->fast_iov[0].iov_base = u64_to_user_ptr(kbuf->addr);
		kmsg->fast_iov[0].iov_len = req->sr_msg.len;
		iov_iter_init(&kmsg->msg.msg_iter, READ, kmsg->fast_iov,
				1, req->sr_msg.len);
	}

	flags = req->sr_msg.msg_flags;
	if (force_nonblock)
		flags |= MSG_DONTWAIT;
	if (flags & MSG_WAITALL)
		min_ret = iov_iter_count(&kmsg->msg.msg_iter);

	ret = __sys_recvmsg_sock(sock, &kmsg->msg, req->sr_msg.umsg,
					kmsg->uaddr, flags);
	if (ret < min_ret) {
		if (ret == -EAGAIN && force_nonblock)
			return io_setup_async_msg(req, kmsg);
		if (ret == -ERESTARTSYS)
			ret = -EINTR;
		if (ret > 0 && io_net_retry(sock, flags)) {
			sr->done_io += ret;
			req->flags |= REQ_F_PARTIAL_IO;
			return io_setup_async_msg(req, kmsg);
		}
		req_set_fail(req);
	} else if ((flags & MSG_WAITALL) && (kmsg->msg.msg_flags & (MSG_TRUNC | MSG_CTRUNC))) {
		req_set_fail(req);
	}

	/* fast path, check for non-NULL to avoid function call */
	if (kmsg->free_iov)
		kfree(kmsg->free_iov);
	req->flags &= ~REQ_F_NEED_CLEANUP;
	if (ret >= 0)
		ret += sr->done_io;
	else if (sr->done_io)
		ret = sr->done_io;
	__io_req_complete(req, issue_flags, ret, io_put_kbuf(req, issue_flags));
	return 0;
}