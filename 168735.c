static DWORD drive_map_windows_err(DWORD fs_errno)
{
	DWORD rc;

	/* try to return NTSTATUS version of error code */

	switch (fs_errno)
	{
		case STATUS_SUCCESS:
			rc = STATUS_SUCCESS;
			break;

		case ERROR_ACCESS_DENIED:
		case ERROR_SHARING_VIOLATION:
			rc = STATUS_ACCESS_DENIED;
			break;

		case ERROR_FILE_NOT_FOUND:
			rc = STATUS_NO_SUCH_FILE;
			break;

		case ERROR_BUSY_DRIVE:
			rc = STATUS_DEVICE_BUSY;
			break;

		case ERROR_INVALID_DRIVE:
			rc = STATUS_NO_SUCH_DEVICE;
			break;

		case ERROR_NOT_READY:
			rc = STATUS_NO_SUCH_DEVICE;
			break;

		case ERROR_FILE_EXISTS:
		case ERROR_ALREADY_EXISTS:
			rc = STATUS_OBJECT_NAME_COLLISION;
			break;

		case ERROR_INVALID_NAME:
			rc = STATUS_NO_SUCH_FILE;
			break;

		case ERROR_INVALID_HANDLE:
			rc = STATUS_INVALID_HANDLE;
			break;

		case ERROR_NO_MORE_FILES:
			rc = STATUS_NO_MORE_FILES;
			break;

		case ERROR_DIRECTORY:
			rc = STATUS_NOT_A_DIRECTORY;
			break;

		case ERROR_PATH_NOT_FOUND:
			rc = STATUS_OBJECT_PATH_NOT_FOUND;
			break;

		default:
			rc = STATUS_UNSUCCESSFUL;
			WLog_ERR(TAG, "Error code not found: %" PRIu32 "", fs_errno);
			break;
	}

	return rc;
}