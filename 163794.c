static int get_filter(void __user *arg, struct sock_filter **p)
{
	struct sock_fprog uprog;
	struct sock_filter *code = NULL;
	int len, err;

	if (copy_from_user(&uprog, arg, sizeof(uprog)))
		return -EFAULT;

	if (!uprog.len) {
		*p = NULL;
		return 0;
	}

	len = uprog.len * sizeof(struct sock_filter);
	code = kmalloc(len, GFP_KERNEL);
	if (code == NULL)
		return -ENOMEM;

	if (copy_from_user(code, uprog.filter, len)) {
		kfree(code);
		return -EFAULT;
	}

	err = sk_chk_filter(code, uprog.len);
	if (err) {
		kfree(code);
		return err;
	}

	*p = code;
	return uprog.len;
}