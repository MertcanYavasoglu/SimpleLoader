#include "../include/api_hashing.h"
#include <cstdio>

// djb2
DWORD GetHash(const char* str) {
    DWORD hash = 5381;
    int c;
    while (c = (unsigned char)*str++)
        hash = ((hash << 5) + hash) + c;
    return hash;
}

FARPROC GetProcAddressByHash(DWORD dwHash) {
    
    // 1. PEB'e Erişim
#if defined(_WIN64)
    PPEB pPeb = (PPEB)__readgsqword(0x60);
#else
    PPEB pPeb = (PPEB)__readfsdword(0x30);
#endif

    PPEB_LDR_DATA pLdr = pPeb->Ldr;
    
    PLIST_ENTRY pHead = &pLdr->InMemoryOrderModuleList;
    PLIST_ENTRY pCurrent = pHead->Flink;

    while (pCurrent != pHead) {
        
        PLDR_DATA_TABLE_ENTRY pDte = (PLDR_DATA_TABLE_ENTRY)((PBYTE)pCurrent - 16);

        if (pDte->DllBase != NULL) {
            
            PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pDte->DllBase;
            
            if (pDosHeader->e_magic == IMAGE_DOS_SIGNATURE) {
                
                PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)((PBYTE)pDte->DllBase + pDosHeader->e_lfanew);
                
                if (pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size > 0) {
                    
                    PIMAGE_EXPORT_DIRECTORY pExportDir = (PIMAGE_EXPORT_DIRECTORY)((PBYTE)pDte->DllBase + pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

                    PDWORD pNames = (PDWORD)((PBYTE)pDte->DllBase + pExportDir->AddressOfNames);
                    PDWORD pFunctions = (PDWORD)((PBYTE)pDte->DllBase + pExportDir->AddressOfFunctions);
                    PWORD pOrdinals = (PWORD)((PBYTE)pDte->DllBase + pExportDir->AddressOfNameOrdinals);

                    for (DWORD i = 0; i < pExportDir->NumberOfNames; i++) {
                        char* szFunctionName = (char*)((PBYTE)pDte->DllBase + pNames[i]);

                        if (szFunctionName != NULL) {
                            if (GetHash(szFunctionName) == dwHash) {
                                // Bulundu!
                                return (FARPROC)((PBYTE)pDte->DllBase + pFunctions[pOrdinals[i]]);
                            }
                        }
                    }
                }
            }
        }
        pCurrent = pCurrent->Flink;
    }

    return NULL;
}