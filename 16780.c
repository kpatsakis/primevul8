R_API st64 r_anal_function_get_var_stackptr_at(RAnalFunction *fcn, st64 delta, ut64 addr) {
	st64 offset = addr - fcn->addr;
	RPVector *inst_accesses = ht_up_find (fcn->inst_vars, offset, NULL);
	if (!inst_accesses) {
		return ST64_MAX;
	}
	RAnalVar *var = NULL;
	void **it;
	r_pvector_foreach (inst_accesses, it) {
		RAnalVar *v = *it;
		if (v->delta == delta) {
			var = v;
			break;
		}
	}
	if (!var) {
		return ST64_MAX;
	}
	size_t index;
	r_vector_lower_bound (&var->accesses, offset, index, ACCESS_CMP);
	RAnalVarAccess *acc = NULL;
	if (index < var->accesses.len) {
		acc = r_vector_index_ptr (&var->accesses, index);
	}
	if (!acc || acc->offset != offset) {
		return ST64_MAX;
	}
	return acc->stackptr;
}