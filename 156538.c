static zend_object_value spl_heap_object_clone(zval *zobject TSRMLS_DC) /* {{{ */
{
	zend_object_value   new_obj_val;
	zend_object        *old_object;
	zend_object        *new_object;
	zend_object_handle  handle = Z_OBJ_HANDLE_P(zobject);
	spl_heap_object    *intern;

	old_object  = zend_objects_get_address(zobject TSRMLS_CC);
	new_obj_val = spl_heap_object_new_ex(old_object->ce, &intern, zobject, 1 TSRMLS_CC);
	new_object  = &intern->std;

	zend_objects_clone_members(new_object, new_obj_val, old_object, handle TSRMLS_CC);

	return new_obj_val;
}