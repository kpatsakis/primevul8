VariationalRefinementImpl::ParallelOp_ParBody::ParallelOp_ParBody(VariationalRefinementImpl &_var, vector<Op> _ops,
                                                                  vector<void *> &_op1s, vector<void *> &_op2s,
                                                                  vector<void *> &_op3s)
    : var(&_var), ops(_ops), op1s(_op1s), op2s(_op2s), op3s(_op3s)
{
}