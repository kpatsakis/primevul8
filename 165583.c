void kvm_apic_clear_irr(struct kvm_vcpu *vcpu, int vec)
{
	apic_clear_irr(vec, vcpu->arch.apic);
}