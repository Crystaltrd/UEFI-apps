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
	efi_gop_mode_info_t *info = NULL;
	uintn_t szinfo = sizeof(efi_gop_mode_info_t);
	if (EFI_ERROR(status)){
		fprintf(stderr,"[E] Error Locating the protocol\n");
		return 1;
	} else 
		printf("[I] GOP Found\n");
	status = gop->QueryMode(gop,gop->Mode ? gop->Mode->Mode : 0, &szinfo, &info);
	if (EFI_ERROR(status)){
		fprintf(stderr,"[E] Error Querying the current mode\n");
		return 1;
	} else 
		printf("[I] Queried the current mode\n");

	printf("Version: %d\n",info->Version);
	printf("Horizontal Resolution: %d\n",info->HorizontalResolution);
	printf("Vertical Resolution: %d\n", info->VerticalResolution);
	printf("Pixels Per Scan Line: %d\n",info->PixelsPerScanLine);
	printf("Pixel bitmask Information: R %x, G %x, B %x, Reserved %x\n",info->PixelInformation.RedMask, info->PixelInformation.GreenMask, info->PixelInformation.BlueMask, info->PixelInformation.ReservedMask);
	return 0;
}

