void __validate_process_creds(struct task_struct *tsk,
			      const char *file, unsigned line)
{
	if (tsk->cred == tsk->real_cred) {
		if (unlikely(read_cred_subscribers(tsk->cred) < 2 ||
			     creds_are_invalid(tsk->cred)))
			goto invalid_creds;
	} else {
		if (unlikely(read_cred_subscribers(tsk->real_cred) < 1 ||
			     read_cred_subscribers(tsk->cred) < 1 ||
			     creds_are_invalid(tsk->real_cred) ||
			     creds_are_invalid(tsk->cred)))
			goto invalid_creds;
	}
	return;

invalid_creds:
	printk(KERN_ERR "CRED: Invalid process credentials\n");
	printk(KERN_ERR "CRED: At %s:%u\n", file, line);

	dump_invalid_creds(tsk->real_cred, "Real", tsk);
	if (tsk->cred != tsk->real_cred)
		dump_invalid_creds(tsk->cred, "Effective", tsk);
	else
		printk(KERN_ERR "CRED: Effective creds == Real creds\n");
	BUG();
}