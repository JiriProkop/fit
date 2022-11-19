#include <iostream>
#include <mutex>
#include <stdio.h>
#include <stdlib.h>
#include <thread>

char *line = NULL;
int score = 0;
bool done = false;
unsigned line_wait = 0;
unsigned main_wait = 0;
std::mutex mut_score;
std::mutex mut_line_wait;
std::mutex mut_main_wait;
std::mutex mut_end_check;
std::mutex mut_end_wait;

void substr(char *str, int add_score) {
    while (done == false) {
        unsigned line_char_cnt = 0;
        unsigned str_char_cnt = 0;
        static unsigned state = 0;
        bool flag = false;

        mut_line_wait.lock();
        line_wait++;
        mut_line_wait.unlock();

        // wait for main
        mut_main_wait.lock();
        mut_main_wait.unlock();

        while (line != NULL && line[line_char_cnt] != '\0') {
            switch (state) {
                case 0:
                    if (line[line_char_cnt++] == str[str_char_cnt++]) {
                        state = 1;
                    } else {
                        str_char_cnt = 0;
                    }
                    break;
                case 1:
                    if (str[str_char_cnt] == '\0') {
                        mut_score.lock();
                        score += add_score;
                        mut_score.unlock();
                        flag = true;

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
                        mut_score.lock();
                        score += add_score;
                        mut_score.unlock();
                        flag = true;

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
                        mut_score.lock();
                        score += add_score;
                        mut_score.unlock();
                        flag = true;

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
                        mut_score.lock();
                        score += add_score;
                        mut_score.unlock();
                        flag = true;

                        str_char_cnt = 0;
                        state = 0;
                    } else if (line[line_char_cnt++] != str[str_char_cnt++]) {
                        str_char_cnt = 0;
                        state = 0;
                        break;
                    } else {
                        state = 5;
                    }
                    break;
                case 5:
                    if (str[str_char_cnt] == '\0') {
                        mut_score.lock();
                        score += add_score;
                        mut_score.unlock();
                        flag = true;
                    }
                    str_char_cnt = 0;
                    state = 0;
                    break;
            } // end of switch
            if (flag) {
                break;
            }
        } // end of while
        mut_end_wait.lock();
        main_wait++;
        mut_end_wait.unlock();

        mut_end_check.lock();
        mut_end_check.unlock();
    }
}

int main(int argc, char **argv) {
    if (argc < 4 || argc % 2 == 1) {
        fprintf(stderr, "Wrong argument count.\n");
        return 1;
    }
    int min_score = atoi(argv[1]);
    unsigned th_cnt = (argc - 2) / 2;
    std::thread ths[th_cnt];
    size_t len = 0;

    mut_main_wait.lock();
    for (unsigned i = 0; i < th_cnt; i++) {
        ths[i] = std::thread(substr, argv[2 * i + 2], atoi(argv[2 * i + 3]));
    }

    while (getline(&line, &len, stdin) != -1) {
        while (line_wait < th_cnt) {
            continue;
        }
        score = 0;
        main_wait = 0;
        mut_end_check.lock();
        mut_main_wait.unlock();

        while (main_wait < th_cnt) {
            continue;
        }
        line_wait = 0;
        if (score >= min_score) {
            std::cout << line;
        }

        mut_main_wait.lock();
        mut_end_check.unlock();
        len = 0;
        free(line);
        line = NULL;
    }
    done = true;

    mut_line_wait.unlock();
    mut_main_wait.unlock();
    mut_end_check.unlock();
    mut_end_wait.unlock();
    for (unsigned i = 0; i < th_cnt; i++) {
        ths[i].join();
    }

    return 0;
}