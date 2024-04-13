netlink_send(int fd, struct cn_msg *msg)
{
	struct nlmsghdr *nlh;
	unsigned int size;
	struct msghdr message;
	char buffer[64];
	struct iovec iov[2];

	size = NLMSG_SPACE(sizeof(struct cn_msg) + msg->len);

	nlh = (struct nlmsghdr *)buffer;
	nlh->nlmsg_seq = 0;
	nlh->nlmsg_pid = getpid();
	nlh->nlmsg_type = NLMSG_DONE;
	nlh->nlmsg_len = NLMSG_LENGTH(size - sizeof(*nlh));
	nlh->nlmsg_flags = 0;

	iov[0].iov_base = nlh;
	iov[0].iov_len = sizeof(*nlh);

	iov[1].iov_base = msg;
	iov[1].iov_len = size;

	memset(&message, 0, sizeof(message));
	message.msg_name = &addr;
	message.msg_namelen = sizeof(addr);
	message.msg_iov = iov;
	message.msg_iovlen = 2;

	return sendmsg(fd, &message, 0);
}