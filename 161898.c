static int selinux_socket_post_create(struct socket *sock, int family,
				      int type, int protocol, int kern)
{
	const struct task_security_struct *tsec = current_security();
	struct inode_security_struct *isec = SOCK_INODE(sock)->i_security;
	struct sk_security_struct *sksec;
	int err = 0;

	isec->sclass = socket_type_to_security_class(family, type, protocol);

	if (kern)
		isec->sid = SECINITSID_KERNEL;
	else {
		err = socket_sockcreate_sid(tsec, isec->sclass, &(isec->sid));
		if (err)
			return err;
	}

	isec->initialized = 1;

	if (sock->sk) {
		sksec = sock->sk->sk_security;
		sksec->sid = isec->sid;
		sksec->sclass = isec->sclass;
		err = selinux_netlbl_socket_post_create(sock->sk, family);
	}

	return err;
}