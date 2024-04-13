static inline long object_common1(UNSERIALIZE_PARAMETER, zend_class_entry *ce)
{
	long elements;
	
	elements = parse_iv2((*p) + 2, p);

	(*p) += 2;
	
	object_init_ex(*rval, ce);
	return elements;
}