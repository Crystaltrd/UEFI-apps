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
	uint32_t rf = 0, gf = 0 , bf = 0;
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

	printf("Mode: %d\n",gop->Mode->Mode);
	printf("Frame Buffer Base Address: 0x%x\n",gop->Mode->FrameBufferBase);
	printf("Frame Buffer Size: %d\n",gop->Mode->FrameBufferSize);
	printf("Information: \n");
	printf("Version: %d\n",info->Version);
	printf("Horizontal Resolution: %d\n",info->HorizontalResolution);
	printf("Vertical Resolution: %d\n", info->VerticalResolution);
	printf("Pixels Per Scan Line: %d\n",info->PixelsPerScanLine);
	printf("Pixel bitmask Information: R 0x%x, G 0x%x, B 0x%x, Reserved 0x%x\n",info->PixelInformation.RedMask, info->PixelInformation.GreenMask, info->PixelInformation.BlueMask, info->PixelInformation.ReservedMask);
	printf("Pixel format: ");
	switch(info->PixelFormat){
		case PixelRedGreenBlueReserved8BitPerColor:
			printf("PixelRedGreenBlueReserved8BitPerColor");
			rf = 0xFF;
			gf = 0xFF00;
			bf = 0xFF0000;
		break;
		case PixelBlueGreenRedReserved8BitPerColor:
			printf("PixelBlueGreenRedReserved8BitPerColor");
			rf = 0xFF0000;
			gf = 0xFF00;
			bf = 0xFF;
		break;
		case PixelBitMask:
			printf("PixelBitMask");
			rf = info->PixelInformation.RedMask;
			gf = info->PixelInformation.GreenMask;
			bf = info->PixelInformation.BlueMask;
		break;
		case PixelBltOnly:
			printf("PixelBltOnly");
		break;
		default:
			fprintf(stderr, "[E] Unknown Pixel format\n");
			return 1;
		break;
	}
	printf("\n");
	printf("r:0x%06x g:0x%06x b:0x%06x\n",rf,gf,bf);
	printf("Writing to 0x0: \n");
	uint32_t pixel = 0;
	for(int y = 0; y < info->VerticalResolution; y++){
	for(int x = 0; x < info->HorizontalResolution; x++){
	*((uint32_t*)(gop->Mode->FrameBufferBase + 4 * info->PixelsPerScanLine * y + 4 * x)) = pixel++;
	}
	}
	return 0;
}

