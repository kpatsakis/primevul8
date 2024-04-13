static void zend_extension_message_dispatcher(const zend_extension *extension, int num_args, va_list args TSRMLS_DC)
{
	int message;
	void *arg;

	if (!extension->message_handler || num_args!=2) {
		return;
	}
	message = va_arg(args, int);
	arg = va_arg(args, void *);
	extension->message_handler(message, arg);
}