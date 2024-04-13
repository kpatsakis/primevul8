VariationalRefinementImpl::ComputeDataTerm_ParBody::ComputeDataTerm_ParBody(VariationalRefinementImpl &_var,
                                                                            int _nstripes, int _h,
                                                                            RedBlackBuffer &_dW_u,
                                                                            RedBlackBuffer &_dW_v, bool _red_pass)
    : var(&_var), nstripes(_nstripes), h(_h), dW_u(&_dW_u), dW_v(&_dW_v), red_pass(_red_pass)
{
    stripe_sz = (int)ceil(h / (double)nstripes);
}