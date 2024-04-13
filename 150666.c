void kvm_vcpu_on_spin(struct kvm_vcpu *me)
{
	struct kvm *kvm = me->kvm;
	struct kvm_vcpu *vcpu;
	int last_boosted_vcpu = me->kvm->last_boosted_vcpu;
	int yielded = 0;
	int pass;
	int i;

	/*
	 * We boost the priority of a VCPU that is runnable but not
	 * currently running, because it got preempted by something
	 * else and called schedule in __vcpu_run.  Hopefully that
	 * VCPU is holding the lock that we need and will release it.
	 * We approximate round-robin by starting at the last boosted VCPU.
	 */
	for (pass = 0; pass < 2 && !yielded; pass++) {
		kvm_for_each_vcpu(i, vcpu, kvm) {
			struct task_struct *task = NULL;
			struct pid *pid;
			if (!pass && i < last_boosted_vcpu) {
				i = last_boosted_vcpu;
				continue;
			} else if (pass && i > last_boosted_vcpu)
				break;
			if (vcpu == me)
				continue;
			if (waitqueue_active(&vcpu->wq))
				continue;
			rcu_read_lock();
			pid = rcu_dereference(vcpu->pid);
			if (pid)
				task = get_pid_task(vcpu->pid, PIDTYPE_PID);
			rcu_read_unlock();
			if (!task)
				continue;
			if (task->flags & PF_VCPU) {
				put_task_struct(task);
				continue;
			}
			if (yield_to(task, 1)) {
				put_task_struct(task);
				kvm->last_boosted_vcpu = i;
				yielded = 1;
				break;
			}
			put_task_struct(task);
		}
	}
}