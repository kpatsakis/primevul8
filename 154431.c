static int handle_tpr_below_threshold(struct kvm_vcpu *vcpu)
{
	kvm_apic_update_ppr(vcpu);
	return 1;
}