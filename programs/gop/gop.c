#include <uefi.h>

int main(int argc, char **argv)
{
	printf("Firmware Vendor: ");
	ST->ConOut->OutputString(ST->ConOut, ST->FirmwareVendor);
	printf("\n");
	printf("Firmware Revision: 0x%xd\n", ST->FirmwareRevision);
	efi_guid_t gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	efi_gop_t *gop = NULL;
	efi_status_t status = BS->LocateProtocol(&gopGuid, NULL, (void**)&gop);
	if (EFI_ERROR(status))
		printf("ERROR!!\n");
	else
		printf("GOP Found\n");
	return 0;
}

