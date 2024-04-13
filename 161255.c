void key_fsgid_changed(struct task_struct *tsk)
{
	/* update the ownership of the thread keyring */
	BUG_ON(!tsk->cred);
	if (tsk->cred->thread_keyring) {
		down_write(&tsk->cred->thread_keyring->sem);
		tsk->cred->thread_keyring->gid = tsk->cred->fsgid;
		up_write(&tsk->cred->thread_keyring->sem);
	}
}