void VariationalRefinementImpl::ParallelOp_ParBody::operator()(const Range &range) const
{
    for (int i = range.start; i < range.end; i++)
        (var->*ops[i])(op1s[i], op2s[i], op3s[i]);
}