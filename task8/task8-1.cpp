/*
Работа 8 -- Сетевое взаимодействие процессов через сокеты
Вариант с сокетами Unix, протоколом TCP и функцией getrusage
Программа 1 -- Клиент
Митяков Д.А.
Группа 0324
*/

#include <bits/types/struct_rusage.h>
#include <fcntl.h>
#include <pthread.h>
#include <queue>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

void* start_connection(void*);
void* send_request(void*);
void* receive_response(void*);

struct response {
    struct rusage data;
    unsigned reqnum;
};

// Thread exit flags
bool connect_f = true;
bool send_f = true;
bool receive_f = true;

// Number of requests
unsigned req_count = 0;

// Mutex to aid keeping request send/receive order
pthread_mutex_t order_mtx;
pthread_t connect_t, send_t, receive_t;

int server_fd;
struct sockaddr_un server_addr;
bool server_connected = false;

void* start_connection(void* args) {
    printf(":: start_connection start ::\n");

    while (connect_f) {
        if (connect(server_fd, (struct sockaddr*)&server_addr,
                    sizeof(struct sockaddr_un)) == -1) {
            perror("connect");
            sleep(2);
        } else {
            printf("Connected to server\n");
            server_connected = true;
            pthread_create(&send_t, NULL, &send_request, NULL);
            pthread_create(&receive_t, NULL, &receive_response, NULL);
            break;
        }
    }

    printf(":: start_connection exit ::\n");
    pthread_exit(NULL);
}

void* send_request(void* args) {
    printf(":: send_request start ::\n");

    while (send_f) {
        pthread_mutex_lock(&order_mtx);

        if (server_connected) { // Send crashes program if no connection?
            if (send(server_fd, &req_count, sizeof(unsigned), 0) == -1)
                perror("send");
            else {
                printf("Sent request #%i\n", req_count);
                req_count++;
            }
        }

        pthread_mutex_unlock(&order_mtx);
        sleep(2);
    }

    printf(":: send_request exit ::\n");
    pthread_exit(NULL);
}

void* receive_response(void* args) {
    printf(":: receive_response start ::\n");

    while (receive_f) {
        pthread_mutex_lock(&order_mtx);

        response res;
        ssize_t count = recv(server_fd, &res, sizeof(response), 0);

        if (count == -1) {
            perror("recv");
        } else if (count == 0) {
            printf("Server disconnected\n");
            server_connected = false;
        } else {
            printf("Received request #%i\n", res.reqnum);
            printf("\tMax resident set size: %ld\n\tPage reclaims: "
                   "%ld\n\tSystem CPU time used: %ld\n",
                   res.data.ru_maxrss, res.data.ru_minflt,
                   (long int)res.data.ru_stime.tv_usec);
        }

        pthread_mutex_unlock(&order_mtx);
        sleep(2);
    }

    printf(":: receive_response exit ::\n");
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    printf(":: main start ::\n");

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(1);
    }

    // Init server_fd
    memset(&server_addr, '\0', sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, "task8sock.soc",
            sizeof(server_addr.sun_path) - 1);

    // Adjust socket options
    int optval = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval,
                   sizeof(optval)) == -1)
        perror("setsockopt");

    pthread_mutex_init(&order_mtx, NULL);

    pthread_create(&connect_t, NULL, &start_connection, NULL);

    printf("Awaiting keypress...\n");
    getchar();
    printf("Received keypress\n");

    connect_f = send_f = receive_f = false;

    if (connect_t != 0)
        pthread_join(connect_t, NULL);
    if (send_t != 0)
        pthread_join(send_t, NULL);
    if (receive_t != 0)
        pthread_join(receive_t, NULL);

    pthread_mutex_destroy(&order_mtx);

    if (shutdown(server_fd, 2) == -1)
        perror("shutdown");
    if (close(server_fd) == -1)
        perror("close");

    printf(":: main exit ::\n");
    return 0;
}
