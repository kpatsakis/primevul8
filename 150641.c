void kvm_reload_remote_mmus(struct kvm *kvm)
{
	make_all_cpus_request(kvm, KVM_REQ_MMU_RELOAD);
}