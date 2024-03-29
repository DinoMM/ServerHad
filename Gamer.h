//
// Created by Daniel on 2. 1. 2024.
//

#ifndef SERVERHAD_GAMER_H
#define SERVERHAD_GAMER_H
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <pthread.h>
#include <csignal>
#include <cerrno>

#define MSG_LEN 16

class Gamer {
private:
    ///w-hore, s-dole, a-vlavo, d-vpravo
    char aktSmer;
    char fruitRow;
    char fruitCol;

    int port;
    int newsockfd;
    int sockfd;
    bool * koniecZberu;
    bool succConnection;
    pthread_t * thread;
    pthread_mutex_t * mutSmer;
    pthread_mutex_t * mutFruit;
    pthread_mutex_t * mutKoniec;
    pthread_mutex_t mutPlay;
    int score;
    bool playing;


public:
    Gamer(pthread_t * vlakno, pthread_mutex_t * mutSmer, pthread_mutex_t * mutKoniec, pthread_mutex_t * mutFruit,  int port, bool * koniecZberu);
    ~Gamer();

    char getAktSmer();
    void setAktSmer(char novySmer);
    char getFruitRow();
    char getFruitCol();
    void setFruit(char fRow, char fCol);
    pthread_mutex_t * getMutexSmer();
    pthread_mutex_t * getMutexKoniec();
    pthread_mutex_t * getMutexPlay();
    pthread_mutex_t * getMutexFruit();
    pthread_t * getThread();
    int getNewsockfd();
    int getSockfd();
    bool * getKoniecZberu();
    bool getSuccConnection();
    int getScore();
    bool isPlaying();



    void setScore(int newScore);

    bool connection();
    void startConnection();


    static void * waitGetData(void * data) {
        Gamer * hrac = (Gamer *)data;

        int status;
        char buffer[MSG_LEN];
        char msg[MSG_LEN];
        bool run = true;
        while(run) {
            bzero(buffer, MSG_LEN);
            status = read(hrac->getNewsockfd(), buffer, MSG_LEN);       //citanie zo socketu (cakanie) mozno zmenit pocet prijimanych bytov
            if (status < 0) {
                perror("Error reading from socket\n");
                return NULL;
            }
            //printf("Here is the message: %s\n", buffer);

            switch (buffer[0]) {            //spracovanie podla prveho znaku (pre vstupy z klavesnice)
                case 'w':
                case 's':
                case 'a':
                case 'd':

                    pthread_mutex_lock(hrac->getMutexSmer());
                    printf("%d: %c\n",hrac->getNewsockfd(),buffer[0]);
                    hrac->setAktSmer(buffer[0]);                //kriticka cast
                    pthread_mutex_unlock(hrac->getMutexSmer());

                    break;
                case 'r':
                    printf("%d: r\n", hrac->getNewsockfd());
                    pthread_mutex_lock(hrac->getMutexKoniec());
                    *hrac->getKoniecZberu() = true;                     //kriticka cast
                    pthread_mutex_unlock(hrac->getMutexKoniec());
                    //printf("Zaznamenanie R na socket %d\n", hrac->getNewsockfd());

                    break;

                default: break;
            }

            switch (buffer[1]) {        //spracovanie specialnych stavov (eventy v hre)
                case 'E':       // pre stav ukoncenia hry (narazenie, ukoncenie hracom)
                    pthread_mutex_lock(hrac->getMutexPlay());
                    hrac->playing = false;                     //kriticka cast
                    pthread_mutex_unlock(hrac->getMutexPlay());

                    //printf("Zaznamenane E\n");
                    break;
                case 'H':
                    if (buffer[3] != '\0') {
                        bzero(msg, MSG_LEN);
                        msg[1] = 'Y';
                        status = write(hrac->getNewsockfd(), msg, MSG_LEN);        //poslanie informacie klientovi
                        //printf("Posielanie H na socket %d\n", hrac->getNewsockfd());
                        if (status < 0) {
                            if (errno == EPIPE) {
                                break;
                            }
                            perror("Error writing to socket\n");
                            return NULL;
                        }
                        hrac->setScore(buffer[3] - '0');

                        pthread_mutex_lock(hrac->getMutexPlay());
                        hrac->playing = false;                     //kriticka cast
                        pthread_mutex_unlock(hrac->getMutexPlay());
                    }
                    break;
                default: break;
            }
            switch (buffer[6]) {
                case 'F':
                    //printf("Fruit %d: %c  %c\n", hrac->newsockfd, buffer[7], buffer[8]);
                    pthread_mutex_lock(hrac->getMutexFruit());
                    hrac->setFruit(buffer[7], buffer[8]);
                    pthread_mutex_unlock(hrac->getMutexFruit());
                    break;
                default: break;
            }
            pthread_mutex_lock(hrac->getMutexKoniec());
            if (*hrac->getKoniecZberu() || !hrac->isPlaying()) {              //kriticka cast pre prerusenie (koniec hry, narazenie a podob)
                run = false;            //dokonci sa cyklus a ukonci sa beh prijimania
            }
            pthread_mutex_unlock(hrac->getMutexKoniec());
            bzero(msg, MSG_LEN);

        }
        return NULL;
    }

};



#endif //SERVERHAD_GAMER_H
