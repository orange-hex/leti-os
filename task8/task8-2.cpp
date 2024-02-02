/*
Работа 8 -- Сетевое взаимодействие процессов через сокеты
Вариант с сокетами Unix, протоколом TCP и функцией getrusage
Программа 2 -- Сервер
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
#include <errno.h>

void* await_connection(void*);
void* accept_request(void*);
void* process_request(void*);
void* send_response(void*);

struct response {
    struct rusage data;
    unsigned reqnum;
};

// Thread exit flags
bool await_f = true;
bool accept_f = true;
bool process_f = true;
bool send_f = true;

int listener_fd;
int client_fd;
struct sockaddr_un listener_addr;
struct sockaddr_un client_addr;
socklen_t addr_size = sizeof(struct sockaddr_un);

pthread_t await_t, accept_t, process_t, send_t;

pthread_mutex_t requests_mtx;
pthread_mutex_t responses_mtx;
std::queue<unsigned> requests;
std::queue<response> responses;

void* await_connection(void* args) {
    printf(":: await_connection start ::\n");

    while (await_f) {
        client_fd = accept(listener_fd, (struct sockaddr*)&client_addr, &addr_size);

        if (client_fd == -1) {
            perror("accept");
            sleep(2);
        } else {
            printf("Client connected\n");
            pthread_create(&accept_t, NULL, &accept_request, NULL);
            pthread_create(&process_t, NULL, &process_request, NULL);
            pthread_create(&send_t, NULL, &send_response, NULL);
            break;
        }
    }

    printf(":: await_connection exit ::\n");
    pthread_exit(NULL);
}

void* accept_request(void* args) {
    printf(":: accept_request start ::\n");

    while (accept_f) {
        unsigned request;

        ssize_t count = recv(client_fd, &request, sizeof(unsigned), 0);

        if (count == -1) {
            perror("recv");
            sleep(1);
        } else if (count == 0) {
            printf("Client disconnected\n");
            sleep(2);
        } else {
            pthread_mutex_lock(&requests_mtx);
            requests.push(request);
            pthread_mutex_unlock(&requests_mtx);

            printf("Received request #%i\n", request);
        }
    }

    printf(":: accept_request exit ::\n");
    pthread_exit(NULL);
}

void* process_request(void* args) {
    printf(":: process_request start ::\n");

    while (process_f) {
        pthread_mutex_lock(&requests_mtx);

        if (!requests.empty()) {
            response res;

            if (getrusage(RUSAGE_SELF, &res.data) == 0) {
                res.reqnum = requests.front();
                requests.pop();
                pthread_mutex_unlock(&requests_mtx);

                pthread_mutex_lock(&responses_mtx);
                responses.push(res);
                pthread_mutex_unlock(&responses_mtx);
            } else {
                pthread_mutex_unlock(&requests_mtx);
                perror("getrusage");
            }
        } else {
            pthread_mutex_unlock(&requests_mtx);
            sleep(1);
        }
    }

    printf(":: process_request exit ::\n");
    pthread_exit(NULL);
}

void* send_response(void* args) {
    printf(":: send_response start ::\n");

    while (process_f) {
        pthread_mutex_lock(&responses_mtx);

        if (!responses.empty()) {
            response res = responses.front();

            if (send(client_fd, &res, sizeof(response), 0) == -1)
                perror("send");
            else {
                responses.pop();
                printf("Responded to request #%i with:\n", res.reqnum);
                printf("\tMax resident set size: %ld\n\tPage reclaims: "
                        "%ld\n\tSystem CPU time used: %ld\n",
                        res.data.ru_maxrss, res.data.ru_minflt,
                        (long int)res.data.ru_stime.tv_usec);
            }
        }

        pthread_mutex_unlock(&responses_mtx);
        sleep(1);
    }

    printf(":: send_response exit ::\n");
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    printf(":: main start ::\n");

    listener_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (listener_fd == -1) {
        perror("socket");
        exit(1);
    }

    // Adjust socket options
    int optval = 1;
    if (setsockopt(listener_fd, SOL_SOCKET, SO_REUSEADDR, &optval,
                   sizeof(optval)) == -1)
        perror("setsockopt");
    
    // Init listener_addr
    memset(&listener_addr, '\0', sizeof(struct sockaddr_un));
    listener_addr.sun_family = AF_UNIX;
    strncpy(listener_addr.sun_path, "task8sock.soc",
            sizeof(listener_addr.sun_path) - 1);

    unlink("task8sock.soc");

    if (bind(listener_fd, (struct sockaddr*)&listener_addr,
             sizeof(struct sockaddr_un))) {
        perror("bind");
        exit(1);
    }

    if (listen(listener_fd, 1))
        perror("listen");

    pthread_mutex_init(&requests_mtx, NULL);
    pthread_mutex_init(&responses_mtx, NULL);

    pthread_create(&await_t, NULL, &await_connection, NULL);

    printf("Awaiting keypress...\n");
    getchar();
    printf("Received keypress\n");

    await_f = accept_f = process_f = send_f = false;

    if (await_t != 0)
        pthread_join(await_t, NULL);
    if (accept_t != 0)
        pthread_join(accept_t, NULL);
    if (process_t != 0)
        pthread_join(process_t, NULL);
    if (send_t != 0)
        pthread_join(send_t, NULL);

    pthread_mutex_destroy(&requests_mtx);
    pthread_mutex_destroy(&responses_mtx);

    if (shutdown(client_fd, 2) == -1)
        perror("shutdown");

    if (close(client_fd) == -1)
        perror("close");

    if (close(listener_fd) == -1)
        perror("close");

    printf(":: main exit ::\n");
    return 0;
}
