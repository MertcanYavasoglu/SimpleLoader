# SimpleLoader

- Process Injection: Early Bird APC injection.
- Bypass Technique 1: API hashing
- Bypass Technique 2: Shellcode encryption
- Honorary Mention: Didn't give RWX at the same time. First Write, then Execute

The implementation of the RunAPC() and CreateTargetProcess() functions was inspired by examples from MalDev Academy. Examples from the WinAPI documentation were used for window opening.

I performed API hashing with djb2. While doing this, I heavily followed this [medium](https://securitymaven.medium.com/api-hashing-why-malware-loves-and-you-should-care-77c5135d9aaa) article.

# Setup and Compile

Shellcode is generated using tools/generator.py. The calc.bin file is created. For this example, I am using harmless calc.exe shellcode. 
The calc.bin file is XORed with a random key using tools/encryptor.py.

```bash
python3 encryptor.py -f calc.bin -a xor -k random -o csharp
```

The XOR-encrypted shellcode in the most recently generated calc.bin file and the random key used in the XOR operation are written to the relevant section in src/injection.cpp.

<img width="813" height="99" alt="image" src="https://github.com/user-attachments/assets/01c1767d-1040-426c-88fb-ec12ff213ff8" />

<img width="781" height="100" alt="image" src="https://github.com/user-attachments/assets/4c7bdc30-fe85-4224-aabb-f0f108646069" />

Compilation with Visual Studio x64 Native Tools Command Prompt, cl.exe:

```DOS
rc.exe /fo resources/resource.res resources/resource.rc
cl.exe /nologo /O2 /W3 /EHsc src/main.cpp src/injection.cpp src/api_hashing.cpp resources/resource.res /Fe:ProtonVPN_v4.3.7_x64.exe /link /SUBSYSTEM:WINDOWS user32.lib kernel32.lib gdi32.lib comctl32.lib
```






