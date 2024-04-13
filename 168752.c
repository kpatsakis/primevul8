static UINT drive_process_irp_query_volume_information(DRIVE_DEVICE* drive, IRP* irp)
{
	UINT32 FsInformationClass;
	wStream* output = NULL;
	char* volumeLabel = { "FREERDP" };
	char* diskType = { "FAT32" };
	WCHAR* outStr = NULL;
	int length;
	DWORD lpSectorsPerCluster;
	DWORD lpBytesPerSector;
	DWORD lpNumberOfFreeClusters;
	DWORD lpTotalNumberOfClusters;
	WIN32_FILE_ATTRIBUTE_DATA wfad;

	if (!drive || !irp)
		return ERROR_INVALID_PARAMETER;

	output = irp->output;

	if (Stream_GetRemainingLength(irp->input) < 4)
		return ERROR_INVALID_DATA;

	Stream_Read_UINT32(irp->input, FsInformationClass);
	GetDiskFreeSpaceW(drive->path, &lpSectorsPerCluster, &lpBytesPerSector, &lpNumberOfFreeClusters,
	                  &lpTotalNumberOfClusters);

	switch (FsInformationClass)
	{
		case FileFsVolumeInformation:

			/* http://msdn.microsoft.com/en-us/library/cc232108.aspx */
			if ((length = ConvertToUnicode(sys_code_page, 0, volumeLabel, -1, &outStr, 0) * 2) <= 0)
			{
				WLog_ERR(TAG, "ConvertToUnicode failed!");
				return CHANNEL_RC_NO_MEMORY;
			}

			Stream_Write_UINT32(output, 17 + length); /* Length */

			if (!Stream_EnsureRemainingCapacity(output, 17 + length))
			{
				WLog_ERR(TAG, "Stream_EnsureRemainingCapacity failed!");
				free(outStr);
				return CHANNEL_RC_NO_MEMORY;
			}

			GetFileAttributesExW(drive->path, GetFileExInfoStandard, &wfad);
			Stream_Write_UINT32(output, wfad.ftCreationTime.dwLowDateTime); /* VolumeCreationTime */
			Stream_Write_UINT32(output,
			                    wfad.ftCreationTime.dwHighDateTime);      /* VolumeCreationTime */
			Stream_Write_UINT32(output, lpNumberOfFreeClusters & 0xffff); /* VolumeSerialNumber */
			Stream_Write_UINT32(output, length);                          /* VolumeLabelLength */
			Stream_Write_UINT8(output, 0);                                /* SupportsObjects */
			/* Reserved(1), MUST NOT be added! */
			Stream_Write(output, outStr, length); /* VolumeLabel (Unicode) */
			free(outStr);
			break;

		case FileFsSizeInformation:
			/* http://msdn.microsoft.com/en-us/library/cc232107.aspx */
			Stream_Write_UINT32(output, 24); /* Length */

			if (!Stream_EnsureRemainingCapacity(output, 24))
			{
				WLog_ERR(TAG, "Stream_EnsureRemainingCapacity failed!");
				return CHANNEL_RC_NO_MEMORY;
			}

			Stream_Write_UINT64(output, lpTotalNumberOfClusters); /* TotalAllocationUnits */
			Stream_Write_UINT64(output, lpNumberOfFreeClusters);  /* AvailableAllocationUnits */
			Stream_Write_UINT32(output, lpSectorsPerCluster);     /* SectorsPerAllocationUnit */
			Stream_Write_UINT32(output, lpBytesPerSector);        /* BytesPerSector */
			break;

		case FileFsAttributeInformation:

			/* http://msdn.microsoft.com/en-us/library/cc232101.aspx */
			if ((length = ConvertToUnicode(sys_code_page, 0, diskType, -1, &outStr, 0) * 2) <= 0)
			{
				WLog_ERR(TAG, "ConvertToUnicode failed!");
				return CHANNEL_RC_NO_MEMORY;
			}

			Stream_Write_UINT32(output, 12 + length); /* Length */

			if (!Stream_EnsureRemainingCapacity(output, 12 + length))
			{
				free(outStr);
				WLog_ERR(TAG, "Stream_EnsureRemainingCapacity failed!");
				return CHANNEL_RC_NO_MEMORY;
			}

			Stream_Write_UINT32(output, FILE_CASE_SENSITIVE_SEARCH | FILE_CASE_PRESERVED_NAMES |
			                                FILE_UNICODE_ON_DISK); /* FileSystemAttributes */
			Stream_Write_UINT32(output, MAX_PATH);                 /* MaximumComponentNameLength */
			Stream_Write_UINT32(output, length);                   /* FileSystemNameLength */
			Stream_Write(output, outStr, length);                  /* FileSystemName (Unicode) */
			free(outStr);
			break;

		case FileFsFullSizeInformation:
			/* http://msdn.microsoft.com/en-us/library/cc232104.aspx */
			Stream_Write_UINT32(output, 32); /* Length */

			if (!Stream_EnsureRemainingCapacity(output, 32))
			{
				WLog_ERR(TAG, "Stream_EnsureRemainingCapacity failed!");
				return CHANNEL_RC_NO_MEMORY;
			}

			Stream_Write_UINT64(output, lpTotalNumberOfClusters); /* TotalAllocationUnits */
			Stream_Write_UINT64(output,
			                    lpNumberOfFreeClusters); /* CallerAvailableAllocationUnits */
			Stream_Write_UINT64(output, lpNumberOfFreeClusters); /* AvailableAllocationUnits */
			Stream_Write_UINT32(output, lpSectorsPerCluster);    /* SectorsPerAllocationUnit */
			Stream_Write_UINT32(output, lpBytesPerSector);       /* BytesPerSector */
			break;

		case FileFsDeviceInformation:
			/* http://msdn.microsoft.com/en-us/library/cc232109.aspx */
			Stream_Write_UINT32(output, 8); /* Length */

			if (!Stream_EnsureRemainingCapacity(output, 8))
			{
				WLog_ERR(TAG, "Stream_EnsureRemainingCapacity failed!");
				return CHANNEL_RC_NO_MEMORY;
			}

			Stream_Write_UINT32(output, FILE_DEVICE_DISK); /* DeviceType */
			Stream_Write_UINT32(output, 0);                /* Characteristics */
			break;

		default:
			irp->IoStatus = STATUS_UNSUCCESSFUL;
			Stream_Write_UINT32(output, 0); /* Length */
			break;
	}

	return irp->Complete(irp);
}