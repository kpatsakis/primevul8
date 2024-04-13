rb_fiddle_new_function(VALUE address, VALUE arg_types, VALUE ret_type)
{
    VALUE argv[3];

    argv[0] = address;
    argv[1] = arg_types;
    argv[2] = ret_type;

    return rb_class_new_instance(3, argv, cFiddleFunction);
}