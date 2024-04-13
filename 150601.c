void kvm_flush_remote_tlbs(struct kvm *kvm)
{
	int dirty_count = kvm->tlbs_dirty;

	smp_mb();
	if (make_all_cpus_request(kvm, KVM_REQ_TLB_FLUSH))
		++kvm->stat.remote_tlb_flush;
	cmpxchg(&kvm->tlbs_dirty, dirty_count, 0);
}