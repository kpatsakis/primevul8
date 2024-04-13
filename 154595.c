static inline bool kvm_vcpu_trigger_posted_interrupt(struct kvm_vcpu *vcpu,
						     bool nested)
{
#ifdef CONFIG_SMP
	int pi_vec = nested ? POSTED_INTR_NESTED_VECTOR : POSTED_INTR_VECTOR;

	if (vcpu->mode == IN_GUEST_MODE) {
		/*
		 * The vector of interrupt to be delivered to vcpu had
		 * been set in PIR before this function.
		 *
		 * Following cases will be reached in this block, and
		 * we always send a notification event in all cases as
		 * explained below.
		 *
		 * Case 1: vcpu keeps in non-root mode. Sending a
		 * notification event posts the interrupt to vcpu.
		 *
		 * Case 2: vcpu exits to root mode and is still
		 * runnable. PIR will be synced to vIRR before the
		 * next vcpu entry. Sending a notification event in
		 * this case has no effect, as vcpu is not in root
		 * mode.
		 *
		 * Case 3: vcpu exits to root mode and is blocked.
		 * vcpu_block() has already synced PIR to vIRR and
		 * never blocks vcpu if vIRR is not cleared. Therefore,
		 * a blocked vcpu here does not wait for any requested
		 * interrupts in PIR, and sending a notification event
		 * which has no effect is safe here.
		 */

		apic->send_IPI_mask(get_cpu_mask(vcpu->cpu), pi_vec);
		return true;
	}
#endif
	return false;
}