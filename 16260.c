static int io_recv(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_buffer *kbuf;
	struct io_sr_msg *sr = &req->sr_msg;
	struct msghdr msg;
	void __user *buf = sr->buf;
	struct socket *sock;
	struct iovec iov;
	unsigned flags;
	int ret, min_ret = 0;
	bool force_nonblock = issue_flags & IO_URING_F_NONBLOCK;

	sock = sock_from_file(req->file);
	if (unlikely(!sock))
		return -ENOTSOCK;

	if (req->flags & REQ_F_BUFFER_SELECT) {
		kbuf = io_recv_buffer_select(req, issue_flags);
		if (IS_ERR(kbuf))
			return PTR_ERR(kbuf);
		buf = u64_to_user_ptr(kbuf->addr);
	}

	ret = import_single_range(READ, buf, sr->len, &iov, &msg.msg_iter);
	if (unlikely(ret))
		goto out_free;

	msg.msg_name = NULL;
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	msg.msg_namelen = 0;
	msg.msg_iocb = NULL;
	msg.msg_flags = 0;

	flags = req->sr_msg.msg_flags;
	if (force_nonblock)
		flags |= MSG_DONTWAIT;
	if (flags & MSG_WAITALL)
		min_ret = iov_iter_count(&msg.msg_iter);

	ret = sock_recvmsg(sock, &msg, flags);
	if (ret < min_ret) {
		if (ret == -EAGAIN && force_nonblock)
			return -EAGAIN;
		if (ret == -ERESTARTSYS)
			ret = -EINTR;
		if (ret > 0 && io_net_retry(sock, flags)) {
			sr->len -= ret;
			sr->buf += ret;
			sr->done_io += ret;
			req->flags |= REQ_F_PARTIAL_IO;
			return -EAGAIN;
		}
		req_set_fail(req);
	} else if ((flags & MSG_WAITALL) && (msg.msg_flags & (MSG_TRUNC | MSG_CTRUNC))) {
out_free:
		req_set_fail(req);
	}

	if (ret >= 0)
		ret += sr->done_io;
	else if (sr->done_io)
		ret = sr->done_io;
	__io_req_complete(req, issue_flags, ret, io_put_kbuf(req, issue_flags));
	return 0;
}