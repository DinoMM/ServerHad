//
// Created by Daniel on 2. 1. 2024.
//

#include "Gamer.h"

Gamer::Gamer(pthread_t * thread, pthread_mutex_t * mutSmer, pthread_mutex_t * mutKoniec, int port, bool * koniecZberu) {
    this->thread = thread;
    this->mutSmer = mutSmer;
    this->mutKoniec = mutKoniec;
    this->aktSmer = 'w';
    this->port = port;
    this->koniecZberu = koniecZberu;
    this->score = -1;

    pthread_mutex_init(&mutPlay, NULL);
    playing = true;

    succConnection = connection();

}

void Gamer::startConnection() {
    if (succConnection) {
        char msg[MSG_LEN];
        msg[1] = 'S';
        int status = write(getNewsockfd(), msg, MSG_LEN);        //poslanie informacie o zacati hry
        //printf("Posielanie S na socket %d\n",getNewsockfd());
        if (status < 0) {
            perror("Error writing to socket\n");
            return;
        }

        pthread_create(thread, NULL, Gamer::waitGetData, this);     //vytvorenie vlakna
    }
}

char Gamer::getAktSmer() {
    return this->aktSmer;
}
void Gamer::setAktSmer(char novySmer) {
    this->aktSmer = novySmer;
}
pthread_mutex_t * Gamer::getMutexSmer() {
    return this->mutSmer;
}
pthread_mutex_t * Gamer::getMutexKoniec() {
    return this->mutKoniec;
}
pthread_t * Gamer::getThread() {
    return this->thread;
}
int Gamer::getNewsockfd() {
    return this->newsockfd;
}
int Gamer::getSockfd() {
    return this->sockfd;
}
bool * Gamer::getKoniecZberu() {
    return this->koniecZberu;
}
bool Gamer::getSuccConnection() {
    return succConnection;
}
int Gamer::getScore() {
    return this->score;
}
pthread_mutex_t * Gamer::getMutexPlay() {
    return &this->mutPlay;
}


void Gamer::setScore(int newScore) {
    this->score = newScore;
}

bool Gamer::isPlaying() {
    bool hodn = false;
    pthread_mutex_lock(&mutPlay);
    hodn = playing;
    pthread_mutex_unlock(&mutPlay);
    return hodn;
}


bool Gamer::connection() {
    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;

    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error creating socket. Step: 1\n");
        return false;
    }

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error binding socket address. Step: 2\n");
        return false;
    }

    listen(sockfd, 5);
    cli_len = sizeof(cli_addr);

    newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &cli_len);  //cakanie na pripojenie
    if (newsockfd < 0)
    {
        perror("ERROR on accept the client. Step: 3\n");
        return false;
    }
    printf("uspesny pripojeny hrac\n");
    return true;
}

Gamer::~Gamer() {
    close(newsockfd);
    close(sockfd);
    pthread_mutex_destroy(&mutPlay);

}
