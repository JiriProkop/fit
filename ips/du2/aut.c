#define _POSIX_C_SOURCE 200809L

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //Header file for sleep(). man 3 sleep for details.

char *line = NULL;
int score = 0;
bool done = false;
unsigned line_wait = 0;
unsigned main_wait = 0;
pthread_mutex_t mut_score;
pthread_mutex_t mut_line_wait;
pthread_mutex_t mut_main_wait;
pthread_mutex_t mut_end_check;
pthread_mutex_t mut_end_wait;
int g_argc;
char **g_argv;

void *substr(char *str, int add_score) {
    int cjdfj = 0; // FIXME
    while (done == false) {
        unsigned line_char_cnt = 0;
        unsigned str_char_cnt = 0;
        static unsigned state = 0;

        pthread_mutex_lock(&mut_line_wait);
        line_wait++;
        pthread_mutex_unlock(&mut_line_wait);
        printf("before waiting for main\n");

        // wait for main
        pthread_mutex_lock(&mut_main_wait);
		printf("in waiting for main\n");
        pthread_mutex_unlock(&mut_main_wait);

		printf("after waiting for main\n");

        while (line != NULL && line[line_char_cnt] != '\0') {
            switch (state) {
                case 0:
                    if (line[line_char_cnt++] == str[str_char_cnt++]) {
                        if (line[line_char_cnt] == '\0') {
                            if (str[str_char_cnt] == '\0') {
                                // critical zone
                                pthread_mutex_lock(&mut_score);
                                score += add_score;
                                pthread_mutex_unlock(&mut_score);
                            }
                        } else {
                            state = 1;
                        }
                    } else {
                        line_char_cnt++;
                    }
                    break;
                case 1:
                    if (str[str_char_cnt] == '\0') {
                        pthread_mutex_lock(&mut_score);
                        score += add_score;
                        pthread_mutex_unlock(&mut_score);
                        str_char_cnt = 0;
                        state = 0;
                    } else if (line[line_char_cnt++] != str[str_char_cnt++]) {
                        str_char_cnt = 0;
                        state = 0;
                        break;
                    } else {
                        state = 2;
                    }
                    break;
                case 2:
                    if (str[str_char_cnt] == '\0') {
                        pthread_mutex_lock(&mut_score);
                        score += add_score;
                        pthread_mutex_unlock(&mut_score);
                        str_char_cnt = 0;
                        state = 0;
                    } else if (line[line_char_cnt++] != str[str_char_cnt++]) {
                        str_char_cnt = 0;
                        state = 0;
                        break;
                    } else {
                        state = 3;
                    }
                    break;
                case 3:
                    if (str[str_char_cnt] == '\0') {
                        pthread_mutex_lock(&mut_score);
                        score += add_score;
                        pthread_mutex_unlock(&mut_score);
                        str_char_cnt = 0;
                        state = 0;
                    } else if (line[line_char_cnt++] != str[str_char_cnt++]) {
                        str_char_cnt = 0;
                        state = 0;
                        break;
                    } else {
                        state = 4;
                    }
                    break;
                case 4:
                    if (str[str_char_cnt] == '\0') {
                        pthread_mutex_lock(&mut_score);
                        score += add_score;
                        pthread_mutex_unlock(&mut_score);
                    }
                    str_char_cnt = 0;
                    state = 0;

                    break;
            }
        } // end of while
        pthread_mutex_lock(&mut_end_wait);
        main_wait++;
        pthread_mutex_unlock(&mut_end_wait);

        pthread_mutex_lock(&mut_end_check);
        pthread_mutex_unlock(&mut_end_check);
        if (cjdfj++ > 20)
            break;
        printf("HELL!: %u\n", main_wait);
        if (line == NULL) {
            printf("Kill me!\n");
        }
        puts(line);
    }
    exit(0);
}

void mut_init() {
    if (pthread_mutex_init(&mut_score, NULL) != 0 ||
        pthread_mutex_init(&mut_line_wait, NULL) != 0 ||
        pthread_mutex_init(&mut_main_wait, NULL) != 0 ||
        pthread_mutex_init(&mut_end_check, NULL) != 0 ||
        pthread_mutex_init(&mut_end_wait, NULL) != 0) {
        fprintf(stderr, "System error - mutex init. failed!\n");
        exit(1);
    }
}

void mut_destroy() {
    if (pthread_mutex_destroy(&mut_score) != 0 ||
        pthread_mutex_destroy(&mut_line_wait) != 0 ||
        pthread_mutex_destroy(&mut_main_wait) != 0 ||
        pthread_mutex_destroy(&mut_end_check) != 0 ||
        pthread_mutex_destroy(&mut_end_wait) != 0) {
        fprintf(stderr, "System error - mutex delete failed!\n");
        exit(1);
    }
}

int main(int argc, char **argv) {
    if (argc < 4 || argc % 2 == 1) {
        fprintf(stderr, "Wrong argument count.\n");
        return 1;
    }
    int min_score = atoi(argv[1]);
    unsigned th_cnt = (argc - 2) / 2;
    pthread_t ths[th_cnt];
    size_t len = 0;
    g_argc = argc;
    g_argv = argv;

    mut_init();
    pthread_mutex_lock(&mut_main_wait);
    for (unsigned i = 0; i < th_cnt; i++) {
		printf("\nAT b: \n");
        pthread_create(ths + i, NULL, substr,(argv[2 * i + 2], atoi(argv[2 * i + 3])));
		printf("\nWAT: \n");
    }

    while (getline(&line, &len, stdin) != -1) {
        printf("\nWATAHELL1: \n\n");
        while (line_wait < th_cnt) {
            continue;
        }
        score = 0;
        main_wait = 0;
        pthread_mutex_lock(&mut_end_check);
        pthread_mutex_unlock(&mut_main_wait);

        while (main_wait < th_cnt) {
            continue;
        }
        line_wait = 0;
        if (score >= min_score) {
            puts(line);
        }

        pthread_mutex_lock(&mut_main_wait);
        pthread_mutex_unlock(&mut_end_check);
        len = 0;
        free(line);
        line = NULL;
    }
	printf("DONE: \n");
    done = true;

    for (unsigned i = 0; i < th_cnt; i++) {
        pthread_join(ths[i], NULL);
    }
    mut_destroy();
    return 0;
}

//	pthread_create(&thread_id, NULL, myThreadFun, NULL);
//	pthread_join(thread_id, NULL);

// void f(int ID) {
// 	printf("Thread %i started\n",ID);
// }

// int main() {
// 	/*******************************
// 	 * Inicializace threadu a zamku
// 	 * *****************************/
// 	int num=10;
// 	int num_zamky=15;
// 	std::vector <std::thread *> threads; /* pole threadu promenne velikosti */

// 	/* vytvorime zamky */
// 	zamky.resize(num_zamky); /* nastavime si velikost pole zamky */
// 	for(int i=0;i<num_zamky;i++){
// 		std::mutex *new_zamek = new std::mutex();
// 		zamky[i]=new_zamek;
// 		/* Pokud je treba, tak vytvoreny zamek muzeme rovnou zamknout */
// 		(*(zamky[i])).lock();
// 	}

// 	/* vytvorime thready */
// 	threads.resize(num); /* nastavime si velikost pole threads */
// 	for(int i=0;i<num;i++){
// 		std::thread *new_thread = new std::thread (f,i);
// 		threads[i]=new_thread;
// 	}
// 	/**********************************
// 	 * Vlastni vypocet aut
// 	 * ********************************/
// 	int res;
// 	line=read_line(&res);
// 	while (res) {
// 		printf("%s\n",line);
// 		free(line); /* uvolnim pamet */
// 		line=read_line(&res);
// 	}

// 	/**********************************
// 	 * Uvolneni pameti
// 	 * ********************************/

// 	/* provedeme join a uvolnime pamet threads */
// 	for(int i=0;i<num;i++){
// 		(*(threads[i])).join();
// 		delete threads[i];
// 	}
// 	/* uvolnime pamet zamku */
// 	for(int i=0;i<num_zamky;i++){
// 		delete zamky[i];
// 	}

// 	printf("everything finished\n");

// }
