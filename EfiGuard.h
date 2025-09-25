#ifndef __EFIGUARD_GUID_H__
#define __EFIGUARD_GUID_H__

#include <Guid/GlobalVariable.h>

#ifdef __cplusplus
extern "C" {
#endif

// EfiGuard Bootkit Protocol GUID
#define EFI_EFIGUARD_DRIVER_PROTOCOL_GUID \
    { \
        {0} \
    }

// Type of Driver Signature Enforcement bypass to use
typedef enum _EFIGUARD_DSE_BYPASS_TYPE {
    // Do not disable DSE.
    DSE_DISABLE_NONE = 0,

    // Prevent DSE initialization at boot by patching SepInitializeCodeIntegrity.
    // DSE will remain disabled until system reboot.
    // Note: This is easily detected by security products.
    DSE_DISABLE_AT_BOOT = 1,

    // Hook EFI SetVariable() runtime service to enable kernel R/W via a backdoor.
    // Typically used to set g_CiOptions/g_CiEnabled to 0 from user mode (EfiDSEFix).
    DSE_DISABLE_SETVARIABLE_HOOK = 2
} EFIGUARD_DSE_BYPASS_TYPE;

// Kernel read/write backdoor data (used with DSE_DISABLE_SETVARIABLE_HOOK)
#define EFIGUARD_BACKDOOR_VARIABLE_NAME                     L"{1}"
#define EFIGUARD_BACKDOOR_VARIABLE_GUID                     &gEfiGlobalVariableGuid
#define EFIGUARD_BACKDOOR_VARIABLE_ATTRIBUTES               (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS)
#define EFIGUARD_BACKDOOR_VARIABLE_DATASIZE                 sizeof(EFIGUARD_BACKDOOR_DATA)

#define EFIGUARD_BACKDOOR_COOKIE_VALUE                      ({2})

typedef struct _EFIGUARD_BACKDOOR_DATA {
    UINTN  CookieValue;     // Must be EFIGUARD_BACKDOOR_COOKIE_VALUE
    VOID*  KernelAddress;
    union {
        struct {
            UINT64 Byte  : 8;
            UINT64 Word  : 16;
            UINT64 Dword : 32;
            UINT64 Spare : 8;
        } s;
        UINT64 Qword;
        VOID*  UserBuffer;
    } u;
    UINT32 Size;
    BOOLEAN ReadOnly;       // TRUE = read operation; FALSE = write operation
} EFIGUARD_BACKDOOR_DATA;

// Main driver configuration data.
// Updated defaults: auto disable DSE at boot and do not wait for key presses.
typedef struct _EFIGUARD_CONFIGURATION_DATA {
    // Type of DSE bypass to use.
    // Default: DSE_DISABLE_AT_BOOT (auto boot-time DSE disable)
    EFIGUARD_DSE_BYPASS_TYPE DseBypassMethod;

    // Whether to pause for a keypress at the end of each patch stage.
    // Recommended for debugging only.
    // Default: FALSE (no keypress needed; fully automatic)
    BOOLEAN WaitForKeyPress;
} EFIGUARD_CONFIGURATION_DATA;

// Sends configuration data to the driver.
typedef EFI_STATUS (EFIAPI* EFIGUARD_CONFIGURE)(
    IN CONST EFIGUARD_CONFIGURATION_DATA* ConfigurationData
    );

// The EfiGuard bootkit driver protocol.
typedef struct _EFIGUARD_DRIVER_PROTOCOL {
    EFIGUARD_CONFIGURE Configure;
} EFIGUARD_DRIVER_PROTOCOL;

extern EFI_GUID gEfiGuardDriverProtocolGuid;

#ifdef __cplusplus
}
#endif

#endif