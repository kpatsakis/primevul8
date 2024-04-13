void display_help(char *progname)
{
	print_usage_title(stdout, progname);
	printf(_("Main options and reports (report name between square brackets):\n"));
	printf(_("\t-B\tPaging statistics [A_PAGE]\n"));
	printf(_("\t-b\tI/O and transfer rate statistics [A_IO]\n"));
	printf(_("\t-d\tBlock devices statistics [A_DISK]\n"));
	printf(_("\t-F [ MOUNT ]\n"));
	printf(_("\t\tFilesystems statistics [A_FS]\n"));
	printf(_("\t-H\tHugepages utilization statistics [A_HUGE]\n"));
	printf(_("\t-I { <int_list> | SUM | ALL }\n"
		 "\t\tInterrupts statistics [A_IRQ]\n"));
	printf(_("\t-m { <keyword> [,...] | ALL }\n"
		 "\t\tPower management statistics [A_PWR_...]\n"
		 "\t\tKeywords are:\n"
		 "\t\tCPU\tCPU instantaneous clock frequency\n"
		 "\t\tFAN\tFans speed\n"
		 "\t\tFREQ\tCPU average clock frequency\n"
		 "\t\tIN\tVoltage inputs\n"
		 "\t\tTEMP\tDevices temperature\n"
		 "\t\tUSB\tUSB devices plugged into the system\n"));
	printf(_("\t-n { <keyword> [,...] | ALL }\n"
		 "\t\tNetwork statistics [A_NET_...]\n"
		 "\t\tKeywords are:\n"
		 "\t\tDEV\tNetwork interfaces\n"
		 "\t\tEDEV\tNetwork interfaces (errors)\n"
		 "\t\tNFS\tNFS client\n"
		 "\t\tNFSD\tNFS server\n"
		 "\t\tSOCK\tSockets\t(v4)\n"
		 "\t\tIP\tIP traffic\t(v4)\n"
		 "\t\tEIP\tIP traffic\t(v4) (errors)\n"
		 "\t\tICMP\tICMP traffic\t(v4)\n"
		 "\t\tEICMP\tICMP traffic\t(v4) (errors)\n"
		 "\t\tTCP\tTCP traffic\t(v4)\n"
		 "\t\tETCP\tTCP traffic\t(v4) (errors)\n"
		 "\t\tUDP\tUDP traffic\t(v4)\n"
		 "\t\tSOCK6\tSockets\t(v6)\n"
		 "\t\tIP6\tIP traffic\t(v6)\n"
		 "\t\tEIP6\tIP traffic\t(v6) (errors)\n"
		 "\t\tICMP6\tICMP traffic\t(v6)\n"
		 "\t\tEICMP6\tICMP traffic\t(v6) (errors)\n"
		 "\t\tUDP6\tUDP traffic\t(v6)\n"
		 "\t\tFC\tFibre channel HBAs\n"
		 "\t\tSOFT\tSoftware-based network processing\n"));
	printf(_("\t-q\tQueue length and load average statistics [A_QUEUE]\n"));
	printf(_("\t-r [ ALL ]\n"
		 "\t\tMemory utilization statistics [A_MEMORY]\n"));
	printf(_("\t-S\tSwap space utilization statistics [A_MEMORY]\n"));
	printf(_("\t-u [ ALL ]\n"
		 "\t\tCPU utilization statistics [A_CPU]\n"));
	printf(_("\t-v\tKernel tables statistics [A_KTABLES]\n"));
	printf(_("\t-W\tSwapping statistics [A_SWAP]\n"));
	printf(_("\t-w\tTask creation and system switching statistics [A_PCSW]\n"));
	printf(_("\t-y\tTTY devices statistics [A_SERIAL]\n"));
	exit(0);
}