cmsBool IsITUFax(jpeg_saved_marker_ptr ptr)
{
	while (ptr)
	{
        if (ptr -> marker == (JPEG_APP0 + 1) && ptr -> data_length > 5) {

			const char* data = (const char*) ptr -> data;

			if (strcmp(data, "G3FAX") == 0) return TRUE;
		}

		ptr = ptr -> next;
	}

	return FALSE;
}