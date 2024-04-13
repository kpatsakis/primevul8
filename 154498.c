static int vmx_set_identity_map_addr(struct kvm *kvm, u64 ident_addr)
{
	to_kvm_vmx(kvm)->ept_identity_map_addr = ident_addr;
	return 0;
}