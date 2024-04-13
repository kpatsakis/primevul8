static int vmx_pre_enter_smm(struct kvm_vcpu *vcpu, char *smstate)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	vmx->nested.smm.guest_mode = is_guest_mode(vcpu);
	if (vmx->nested.smm.guest_mode)
		nested_vmx_vmexit(vcpu, -1, 0, 0);

	vmx->nested.smm.vmxon = vmx->nested.vmxon;
	vmx->nested.vmxon = false;
	vmx_clear_hlt(vcpu);
	return 0;
}