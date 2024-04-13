void kvm_vcpu_block(struct kvm_vcpu *vcpu)
{
	DEFINE_WAIT(wait);

	for (;;) {
		prepare_to_wait(&vcpu->wq, &wait, TASK_INTERRUPTIBLE);

		if (kvm_arch_vcpu_runnable(vcpu)) {
			kvm_make_request(KVM_REQ_UNHALT, vcpu);
			break;
		}
		if (kvm_cpu_has_pending_timer(vcpu))
			break;
		if (signal_pending(current))
			break;

		schedule();
	}

	finish_wait(&vcpu->wq, &wait);
}