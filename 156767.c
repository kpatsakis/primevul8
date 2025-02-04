static void kvm_vcpu_do_singlestep(struct kvm_vcpu *vcpu, int *r)
{
	struct kvm_run *kvm_run = vcpu->run;

	if (vcpu->guest_debug & KVM_GUESTDBG_SINGLESTEP) {
		kvm_run->debug.arch.dr6 = DR6_BS | DR6_FIXED_1 | DR6_RTM;
		kvm_run->debug.arch.pc = vcpu->arch.singlestep_rip;
		kvm_run->debug.arch.exception = DB_VECTOR;
		kvm_run->exit_reason = KVM_EXIT_DEBUG;
		*r = EMULATE_USER_EXIT;
	} else {
		kvm_queue_exception_p(vcpu, DB_VECTOR, DR6_BS);
	}
}