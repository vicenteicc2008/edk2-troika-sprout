#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/DevicePath.h>
#include <Protocol/PciIo.h>

typedef struct {
  UINTN Signature;
  EFI_EVENT PeriodicTimer;
  EFI_EVENT OneShotTimer;
  //
  // Other device specific fields
  //
} EXAMPLE_DEVICE;

VOID
TimerHandler (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  //
  // Perform a UEFI driver-specific operation.
  //
}

EFI_STATUS Status;
EXAMPLE_DEVICE *Device;
Status = gBS->CreateEvent (
                EVT_TIMER | EVT_NOTIFY_SIGNAL,  // Type
                TPL_NOTIFY,                     // NotifyTpl
                TimerHandler,                   // NotifyFunction
                Device,                         // NotifyContext
                &Device->PeriodicTimer          // Event
                );
if (EFI_ERROR (Status)) {
  return Status;
}

//
// Program the timer event to be signaled every 100 ms.
//
Status = gBS->SetTimer (
                Device->PeriodicTimer,
                TimerPeriodic,
                EFI_TIMER_PERIOD_MILLISECONDS (4)
                );
if (EFI_ERROR (Status)) {
  return Status;
}

EFI_STATUS Status;
EFI_HANDLE ChildHandle;
EXAMPLE_CONFIGURATION ExampleConfiguration;
EFI_HANDLE PciControllerHandle;
EFI_PCI_IO_PROTOCOL *PciIo;
EFI_DEVICE_PATH_PROTOCOL *PciDevicePath;
MEDIA_RELATIVE_OFFSET_RANGE_DEVICE_PATH OptionRomNode;
EFI_DEVICE_PATH_PROTOCOL *PciOptionRomDevicePath;
EFI_HANDLE NewImageHandle;
UINT8 Value;
DataSize = sizeof (EXAMPLE_CONFIGURATION);
UINTN DataSize;
UINT32 Attributes;
EFI_HANDLE ControllerHandle;
EFI_BLOCK_IO_PROTOCOL *BlockIo;
EFI_BLOCK_IO_MEDIA OldMedia;
EFI_DEVICE_PATH_PROTOCOL *DevicePath;
EFI_HANDLE ParentHandle;
EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRootBridgeIo;

//
// Retrieve the Device Path Protocol instance on ControllerHandle
//
Status = gBS->OpenProtocol (
                ControllerHandle,
                &gEfiDevicePathProtocolGuid,
                (VOID **)&DevicePath,
                gImageHandle,
                ControllerHandle,
                EFI_OPEN_PROTOCOL_GET_PROTOCOL
                );
if (EFI_ERROR (Status)) {
  return Status;
}

//
// Find a parent controller that supports the
// PCI Root Bridge I/O Protocol
//
Status = gBS->LocateDevicePath (
                &gEfiPciRootBridgeIoProtocolGuid,
                &DevicePath,
                &ParentHandle
                );
if (EFI_ERROR (Status)) {
  return Status;
}

//
// Get the PCI Root Bridge I/O Protocol instance on ParentHandle
//
Status = gBS->OpenProtocol (
                ParentHandle,
                &gEfiPciRootBridgeIoProtocolGuid,
                (VOID **)&PciRootBridgeIo,
                gImageHandle,
                ControllerHandle,
                EFI_OPEN_PROTOCOL_GET_PROTOCOL
                );
if (EFI_ERROR (Status)) {
  return Status;
}

//
// Retrieve the Device Path Protocol instance on ControllerHandle
//
Status = gBS->OpenProtocol (
                ControllerHandle,
                &gEfiDevicePathProtocolGuid,
                (VOID **)&DevicePath,
                gImageHandle,
                ControllerHandle,
                EFI_OPEN_PROTOCOL_GET_PROTOCOL
                );
if (EFI_ERROR (Status)) {
  return Status;
}

//
// Check to see if the UART parameters have been modified
// and update UART node of DevicePath
//
//
//
//
Status = gBS->ReinstallProtocolInterface (
                ControllerHandle,
                &gEfiDevicePathProtocolGuid,
                DevicePath,
                DevicePath
                );
if (EFI_ERROR (Status)) {
  return Status;
}
//
// Make a copy of the current Block I/O Media structure
//
CopyMem (&OldMedia, &(BlockIo->Media), sizeof (EFI_BLOCK_IO_MEDIA));

//
// Perform driver specific action(s) required to detect if the
// media has been changed and update Block I/O Media structure.
//
//
// Detect whether it is necessary to reinstall the Block I/O Protocol.
//
if ((BlockIo->Media->MediaId != OldMedia.MediaId) ||
    (BlockIo->Media->MediaPresent != OldMedia.MediaPresent) ||
    (BlockIo->Media->ReadOnly != OldMedia.ReadOnly) ||
    (BlockIo->Media->BlockSize != OldMedia.BlockSize) ||
    (BlockIo->Media->LastBlock != OldMedia.LastBlock) ) {

  Status = gBS->ReinstallProtocolInterface (
                  ControllerHandle,
                  &gEfiBlockIoProtocolGuid,
                  BlockIo,
                  BlockIo
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
}

//
// Recursively connect all drivers to the hot-added device
//
Status = gBS->ConnectController (ChildHandle, NULL, NULL, TRUE);
if (EFI_ERROR (Status)) {
  return Status;
}

//
// Do a single 8-bit MMIO write to BAR #1, Offset 0x10 of 0xAA
//
Value = 0xAA;
Status = PciIo->Mem.Write (
                      PciIo,               // This
                      EfiPciIoWidthUint8,  // Width
                      1,                   // BarIndex
                      0x10,                // Offset
                      1,                   // Count
                      &Value               // Buffer
                      );

//
// Wait 10 uS
//
gBS->Stall (10);
//
// Do a single 8-bit MMIO write to BAR #1, Offset 0x10 of 0x55
//
Value = 0x55;
Status = PciIo->Mem.Write (
                      PciIo,               // This
                      EfiPciIoWidthUint8,  // Width
                      1,                   // BarIndex
                      0x10,                // Offset
                      1,                   // Count
                      &Value               // Buffer
                      );

Attributes = EFI_VARIABLE_NON_VOLATILE |
             EFI_VARIABLE_BOOTSERVICE_ACCESS |
             EFI_VARIABLE_RUNTIME_ACCESS;
Status = gRT->GetVariable (
                L"ExampleConfiguration",                // VariableName
                &gEfiExampleConfigurationVariableGuid,  // VendorGuid
                &Attributes,                            // Attributes
                &DataSize,                              // DataSize
                &ExampleConfiguration                   // Data
                );
if (EFI_ERROR (Status)) {
  return Status;
}



//
// Retrieve PCI I/O Protocol associated with PciControllerHandle
//
Status = gBS->OpenProtocol (
                PciControllerHandle,
                &gEfiPciIoProtocolGuid,
                (VOID **)&PciIo,
                gImageHandle,
                NULL,
                EFI_OPEN_PROTOCOL_GET_PROTOCOL
                );
if (EFI_ERROR (Status)) {
  return Status;
}

//
// Retrieve Device Path Protocol associated with PciControllerHandle
//
Status = gBS->OpenProtocol (
                PciControllerHandle,
                &gEfiDevicePathProtocolGuid,
                (VOID **)&PciDevicePath,
                gImageHandle,
                NULL,
                EFI_OPEN_PROTOCOL_GET_PROTOCOL
                );
if (EFI_ERROR (Status)) {
  return Status;
}

//
// Create Device Path Protocol to UEFI Application in PCI Option ROM
//
OptionRomNode.Header.Type = MEDIA_DEVICE_PATH;
OptionRomNode.Header.SubType = MEDIA_RELATIVE_OFFSET_RANGE_DP;
SetDevicePathNodeLength (&OptionRomNode.Header, sizeof (OptionRomNode));
OptionRomNode.StartingOffset = BASE_32KB;
OptionRomNode.EndingOffset = BASE_64KB - 1;
PciOptionRomDevicePath = AppendDevicePathNode (
                           PciDevicePath,
                           &OptionRomNode.Header
                           );

//
// Load UEFI Image from PCI Option ROM container
//
Status = gBS->LoadImage (
                FALSE,
                gImageHandle,
                PciOptionRomDevicePath,
                (UINT8 *)(PciIo->RomImage) + SIZE_32KB,
                SIZE_32KB,
                &NewImageHandle
                );
if (EFI_ERROR (Status)) {
  return Status;
}

//
// Start UEFI Image from PCI Option ROM container
//
Status = gBS->StartImage (NewImageHandle, NULL, NULL);
if (EFI_ERROR (Status)) {
  return Status;
}
