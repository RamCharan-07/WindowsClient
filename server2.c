#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef _WIN32
#include <winsock2.h>
typedef int socklen_t; // Define socklen_t for Windows
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#endif

#define PORT 8080
#define BACKLOG 5

int server_socket;

void log_event(const char* message) {
    FILE* log_file = fopen("C:\\Users\\s3502\\Documents\\cross_platform_service\\log_file.txt", "a");
    if (log_file == NULL) {
        return;
    }
    time_t now = time(NULL);
    struct tm* local_time = localtime(&now);

    char time_str[100];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local_time);

    fprintf(log_file, "%s: %s\n", time_str, message);
    fclose(log_file);
}

#ifdef _WIN32
SERVICE_STATUS ServiceStatus;
SERVICE_STATUS_HANDLE hStatus;

void ServiceMain(int argc, char** argv);
void ControlHandler(DWORD request);


int InitService() {
    log_event("Service initializing");
    // Initialization code here
    return 0;
}
#endif

void print_time(const char* label, time_t time) {
    char time_str[100];
    struct tm* local_time = localtime(&time);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local_time);
    printf("%s %s\n", label, time_str);
}

void handle_client(int client_socket) {
    time_t connection_time = time(NULL);
    print_time("Client connected at", connection_time);
    log_event("Client Connected");

    // Simulate server processing
    //sleep(100);

    time_t disconnection_time = time(NULL);
    print_time("Client disconnected at", disconnection_time);
    log_event("Client Disconnected");
#ifdef _WIN32
    closesocket(client_socket);
#else
    close(client_socket);
#endif
}

void signal_handler(int sig) {
#ifdef _WIN32
    if (server_socket != INVALID_SOCKET) {
        closesocket(server_socket);
        WSACleanup(); // Cleanup Winsock
    }
#else
    if (server_socket != -1) {
        close(server_socket);
    }
#endif
    exit(0);
}

int run_server() {
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len; // Use socklen_t here

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("WSAStartup failed");
        exit(1);
    }
#endif

    signal(SIGINT, signal_handler);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("ERROR opening socket");
#ifdef _WIN32
        WSACleanup();
#endif
        exit(1);
    }

    memset((char*)&server_addr, 0, sizeof(server_addr)); // Use memset instead of bzero
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("ERROR on binding");
#ifdef _WIN32
        closesocket(server_socket);
        WSACleanup();
#else
        close(server_socket);
#endif
        exit(1);
    }

    listen(server_socket, BACKLOG);
    client_len = sizeof(client_addr);

    printf("Server running. Waiting for connections...\n");

    while (1) {
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket < 0) {
            perror("ERROR on accept");
            continue;
        }

        handle_client(client_socket);
    }

#ifdef _WIN32
    closesocket(server_socket);
    WSACleanup();
#else
    close(server_socket);
#endif

    return 0;
}

#ifdef _WIN32
void ServiceMain(int argc, char** argv) {
    int error;

    ServiceStatus.dwServiceType = SERVICE_WIN32;
    ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
    ServiceStatus.dwWin32ExitCode = 0;
    ServiceStatus.dwServiceSpecificExitCode = 0;
    ServiceStatus.dwCheckPoint = 0;
    ServiceStatus.dwWaitHint = 0;

    hStatus = RegisterServiceCtrlHandler("SimpleTCPServer", (LPHANDLER_FUNCTION)ControlHandler);
    if (hStatus == (SERVICE_STATUS_HANDLE)0) {
        return;
    }

    error = InitService();
    if (error) {
        ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        ServiceStatus.dwWin32ExitCode = -1;
        SetServiceStatus(hStatus, &ServiceStatus);
        return;
    }

    ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(hStatus, &ServiceStatus);

    // Run the server
    run_server();
}

void ControlHandler(DWORD request) {
    switch (request) {
    case SERVICE_CONTROL_STOP:
        ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(hStatus, &ServiceStatus);
        log_event("Service stopped");
        return;

    case SERVICE_CONTROL_SHUTDOWN:
        ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(hStatus, &ServiceStatus);
        log_event("Service shutdown");
        return;

    default:
        break;
    }

    SetServiceStatus(hStatus, &ServiceStatus);
}
#endif

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SERVICE_TABLE_ENTRY ServiceTable[2];
    ServiceTable[0].lpServiceName = "SimpleTCPServer";
    ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;
    ServiceTable[1].lpServiceName = NULL;
    ServiceTable[1].lpServiceProc = NULL;

    if (!StartServiceCtrlDispatcher(ServiceTable)) {
        log_event("StartServiceCtrlDispatcher error");
    }

    return 0;
#else
    pid_t pid, sid;

    // Fork the parent process
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    // If we got a good PID, then we can exit the parent process.
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    // Change the file mode mask
    umask(0);

    // Create a new SID for the child process
    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }

    // Change the current working directory
    if ((chdir("/")) < 0) {
        exit(EXIT_FAILURE);
    }

    // Close out the standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Optionally, redirect standard file descriptors to /dev/null
    int fd0 = open("/dev/null", O_RDWR);
    int fd1 = dup(0);
    int fd2 = dup(0);

    // Run the server
    run_server();

    return 0;
#endif
}

//cl /D _WIN32 server2.c ws2_32.lib Advapi32.lib     compiling server2.c file on visual studio 2022