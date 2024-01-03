#include <iostream>
#include "Gamer.h"



//Server
int main(int argc, char *argv[]) {

    if (argc < 2)           //overenie parametra
    {
        fprintf(stderr,"Nebolo zadane cislo portu: %s\n", argv[0]);
        return 1;
    }

    int uniPort = atoi(argv[1]);

    bool koniecZberu = false;
    pthread_mutex_t mutKonec;
    pthread_mutex_t mutSmer1;
    pthread_mutex_t mutSmer2;
    pthread_mutex_init(&mutKonec, NULL);
    pthread_mutex_init(&mutSmer1, NULL);
    pthread_mutex_init(&mutSmer2, NULL);
    pthread_t tHrac1;
    pthread_t tHrac2;

    signal(SIGPIPE, SIG_IGN);       //potlacuje error ked sa jeden hrac odpoji tak aby nepadol program - je potrebne cez errno okontrolovat ci je tomu naozaj tak

    Gamer * hrac1 = new Gamer(&tHrac1, &mutSmer1, &mutKonec, uniPort, &koniecZberu);
    Gamer * hrac2 = new Gamer(&tHrac2, &mutSmer2, &mutKonec, uniPort + 1, &koniecZberu);

    while (true) {
        if (hrac1->getSuccConnection() && hrac2->getSuccConnection()) {
            break;
        }
    }

        printf("spustenie hry\n");
        hrac1->startConnection();
        hrac2->startConnection();

        bool run = true;
        int status;
        char msg[MSG_LEN];
        while (run) {
            bzero(msg, MSG_LEN);
            pthread_mutex_lock(&mutSmer1);      //hrac1 input pre hraca2
            msg[2] = hrac1->getAktSmer();   //kriticka cast
            pthread_mutex_unlock(&mutSmer1);
            status = write(hrac2->getNewsockfd(), msg, MSG_LEN);        //poslanie informacie druhemu klientovi
            if (status < 0) {
                if (errno == EPIPE) {
                    //printf("broken pipe1\n");
                    pthread_mutex_lock(&mutKonec);
                    koniecZberu = true;
                    pthread_mutex_unlock(&mutKonec);
                } else {
                    perror("Error writing to socket\n");
                    return NULL;
                }
            }

            bzero(msg, MSG_LEN);
            pthread_mutex_lock(&mutSmer2);  //hrac2 input pre hraca1
            msg[2] = hrac2->getAktSmer();   //kriticka cast
            pthread_mutex_unlock(&mutSmer2);
            status = write(hrac1->getNewsockfd(), msg, MSG_LEN);        //poslanie informacie druhemu klientovi
            if (status < 0) {
                if (errno == EPIPE) {
                    //printf("broken pipe2\n");
                    pthread_mutex_lock(&mutKonec);
                    koniecZberu = true;
                    pthread_mutex_unlock(&mutKonec);
                } else {
                    perror("Error writing to socket\n");
                    return NULL;
                }
            }

            pthread_mutex_lock(&mutKonec);
            if (koniecZberu) {              //kriticka cast pre prerusenie (koniec hry, narazenie a podob)
                run = false;
            }
            pthread_mutex_unlock(&mutKonec);

            usleep(100 * 1000);      //tick kazdych:- 33 ms (30 fps) - 100 ms (10 fps)
        }

        //oznamenie hracom ze hra sa skoncila
        bzero(msg, MSG_LEN);
        msg[1] = 'E';       //ukoncenie
        status = write(hrac1->getNewsockfd(), msg, MSG_LEN);        //poslanie informacie druhemu klientovi    strlen(msg) + 1
        if (status < 0) {
            if (errno == EPIPE) {   //toto sa stane ked sa spojenie zrusi, cize uz klient vie ze sa ma hra vypnut 1
                //printf("Broken pipeline2\n");
            } else {
                perror("Error writing to socket\n");
                return NULL;
            }
        }
        status = write(hrac2->getNewsockfd(), msg, MSG_LEN);        //poslanie informacie druhemu klientovi
        if (status < 0) {
            if (errno == EPIPE) { //toto sa stane ked sa spojenie zrusi, cize uz klient vie ze sa ma hra vypnut 2
                //printf("Broken pipeline2\n");
            } else {
                perror("Error writing to socket\n");
                return NULL;
            }
        }
        //printf("preslo oznamenie vsetkym hracom\n");




    //vyhodnotenie vysledkov z hry?



    printf("Pred Finalny koniec spojenia\n");
    if (hrac1->getSuccConnection()) {
        pthread_join(tHrac1, NULL);
    }
    if (hrac2->getSuccConnection()) {
        pthread_join(tHrac2, NULL);
    }
    printf("Finalny koniec spojenia\n");

    pthread_mutex_destroy(&mutKonec);
    pthread_mutex_destroy(&mutSmer1);
    pthread_mutex_destroy(&mutSmer2);
    free(hrac1);
    free(hrac2);

    return 0;
}
