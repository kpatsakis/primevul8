_XimGetNestedListSeparator(
    XIMResourceList	 res_list,		/* LISTofIMATTR or IMATTR */
    unsigned int	 res_num)
{
    return  _XimGetResourceListRec(res_list, res_num, XNSeparatorofNestedList);
}