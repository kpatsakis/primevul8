innodb_unknown_command(
/*===================*/
	ENGINE_HANDLE*	handle,		/*!< in: Engine Handle */
	const void*	cookie,		/*!< in: connection cookie */
	protocol_binary_request_header *request, /*!< in: request */
	ADD_RESPONSE	response)	/*!< out: respondse */
{
	struct innodb_engine* innodb_eng = innodb_handle(handle);
	struct default_engine *def_eng = default_handle(innodb_eng);

	return(def_eng->engine.unknown_command(innodb_eng->default_engine,
					       cookie, request, response));
}