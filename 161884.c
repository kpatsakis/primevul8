static void selinux_sock_graft(struct sock *sk, struct socket *parent)
{
	struct inode_security_struct *isec = SOCK_INODE(parent)->i_security;
	struct sk_security_struct *sksec = sk->sk_security;

	if (sk->sk_family == PF_INET || sk->sk_family == PF_INET6 ||
	    sk->sk_family == PF_UNIX)
		isec->sid = sksec->sid;
	sksec->sclass = isec->sclass;
}