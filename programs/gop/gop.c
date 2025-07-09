#include <uefi.h>
#define MIN(a,b) (((a)<(b))?(a):(b))
#define FLAG_BLUE 	0x0065bd
#define FLAG_YELLOW	0xf2cd00
#define FLAG_RED	0xcc0033

efi_gop_t *gop = NULL;
static inline void PlotPixel_32bpp(int x, int y, uint32_t pixel)
{
   *((uint32_t*)(gop->Mode->FrameBufferBase + 4 * gop->Mode->Information->PixelsPerScanLine * y + 4 * x)) = pixel;
}

int main(int argc, char **argv)
{
	
	efi_gop_mode_info_t *info = NULL;
	printf("Firmware Vendor: ");
	ST->ConOut->OutputString(ST->ConOut, ST->FirmwareVendor);
	printf("\n");
	printf("Firmware Revision: 0x%xd\n", ST->FirmwareRevision);
	efi_guid_t gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	efi_status_t status = BS->LocateProtocol(&gopGuid, NULL, (void**)&gop);
	if (EFI_ERROR(status)){
		fprintf(stderr,"[E] Error Locating the protocol\n");
		return 1;
	} else 
		printf("[I] GOP Found\n");
        if(argc > 1) {
            status = gop->SetMode(gop, atoi(argv[1]));
            ST->ConOut->Reset(ST->ConOut, 0);
            ST->StdErr->Reset(ST->StdErr, 0);
            if(EFI_ERROR(status)) {
                fprintf(stderr, "unable to set video mode\n");
                return 0;
            }
        }
	uintn_t szinfo = sizeof(efi_gop_mode_info_t);
	uint32_t rf = 0, gf = 0 , bf = 0, currmode = 0;
	status = gop->QueryMode(gop,gop->Mode ? gop->Mode->Mode : 0, &szinfo, &info);
	currmode = gop->Mode->Mode;
        if(status == EFI_NOT_STARTED || !gop->Mode) {
            status = gop->SetMode(gop, 0);
            ST->ConOut->Reset(ST->ConOut, 0);
            ST->StdErr->Reset(ST->StdErr, 0);
        }

	if (EFI_ERROR(status)){
		fprintf(stderr,"[E] Error Querying the current mode\n");
		return 1;
	} else 
		printf("[I] Queried the current mode\n");

	for(int i = 0; i < gop->Mode->MaxMode;i++){
		status = gop->QueryMode(gop, i, &szinfo, &info);
           	if(EFI_ERROR(status) || info->PixelFormat > PixelBitMask) continue;
		switch(info->PixelFormat){
		case PixelRedGreenBlueReserved8BitPerColor:
			rf = 0xFF;
			gf = 0xFF00;
			bf = 0xFF0000;
		break;
		case PixelBlueGreenRedReserved8BitPerColor:
			rf = 0xFF0000;
			gf = 0xFF00;
			bf = 0xFF;
		break;
		case PixelBitMask:
			rf = info->PixelInformation.RedMask;
			gf = info->PixelInformation.GreenMask;
			bf = info->PixelInformation.BlueMask;
		break;
		default:
		break;
	}
		printf("%c %d - %dx%d (pitch %4d fmt %d r:0x%06x g:0x%06x b:0x%06x)\n",currmode == i ? '*': ' ',i,info->HorizontalResolution,info->VerticalResolution,info->PixelsPerScanLine,info->PixelFormat,rf,gf,bf);
	}

	printf("Base Buffer: %x, Size %d\n", gop->Mode->FrameBufferBase, gop->Mode->FrameBufferSize);	
	printf("Current: %dx%d pitch %4d fmt %d\n",gop->Mode->Information->HorizontalResolution,gop->Mode->Information->VerticalResolution,gop->Mode->Information->PixelsPerScanLine,gop->Mode->Information->PixelFormat);
	printf("Press any key to continue...\n");
	getchar();
	for(int y = 0; y < 200; y++){
		for(int x = 0; x < 200; x++){
			PlotPixel_32bpp(x,y,0xFF0000);
		}
	}
	getchar();
	return 0;
}

