#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>

// shared variebles
int* oxy_count = NULL;      // number of waiting oxygen atoms
int* hydro_count = NULL;    // number of waiting hydrogen atoms
int* oxy_done = NULL;       // number of atoms of oxygen, already in molecules
int* hydro_done = NULL;     // number of atoms of hydroxygen, already in molecules
int* barrier_count = NULL;  // number of needed atoms to open the barrier
int* action_count = NULL;   // counter of actions for log-ing
int* m_count = NULL;        // counter of molecules for log-ing

// semaphores
sem_t* oxy_queue = NULL;
sem_t* hydro_queue = NULL;
sem_t* atom_mutex = NULL;
sem_t* action_mutex = NULL;

// barrier
sem_t* barrier_mutex = NULL;
sem_t* turnstile1 = NULL;
sem_t* turnstile2 = NULL;

FILE* fp;

// array for storing arguments
int nums[4];

// no - number of oxygen atoms
// nh - number of hydrogen atoms
// ti - max time in miliseconds atom's waiting for molecule creation queuing
// tb - max time in miliseconds needed for one molecule creation
enum ints {no, nh, ti, tb};

// function for distinguishing between atoi returning 0 for error or because the number in str was 0
// returns -1 for error, otherwise desired number
int atoi_with_check(char* str)
{
    if(strcmp(str, "0\0") == 0)
    {
        return 0;
    }

    int n = atoi(str);
    if(n == 0)
    {
        return -1;
    }
    else
    {
        return n;
    }
}

// function for mapping a int varieble into the shared memory
int* map(int* p)
{
    p = (int*) mmap(NULL, sizeof(*p), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    return p;
}

// function for unmapping a int varieble from shared memory
void unmap(int* p)
{
    munmap(p, sizeof(p));
}

void cleanup()
{
    while(wait(NULL) > 0); // ending possible zombie processes

    // closing and unlinking all the semaphores
    sem_close(atom_mutex);
    sem_unlink("/xproko47.ios.projekt2-atom_mutex");

    sem_close(oxy_queue);
    sem_unlink("/xproko47.ios.projekt2-oxy_queue");

    sem_close(hydro_queue);
    sem_unlink("/xproko47.ios.projekt2-hydro_queue");

    sem_close(barrier_mutex);
    sem_unlink("/xproko47.ios.projekt2-barrier_mutex");

    sem_close(turnstile1);
    sem_unlink("/xproko47.ios.projekt2-turnstile1");

    sem_close(turnstile2);
    sem_unlink("/xproko47.ios.projekt2-turnstile2");

    sem_close(action_mutex);
    sem_unlink("/xproko47.ios.projekt2-action_mutex");

    // unmapping shared variebles
    unmap(oxy_count);
    unmap(hydro_count);
    unmap(oxy_done);
    unmap(hydro_done);
    unmap(barrier_count);
    unmap(action_count);
    unmap(m_count);
}

void init()
{
    // testing whether it's possible to open or create the file
    // if it already exists, clearing it
    fp = fopen("proj2.out", "w");
    if(fp == NULL)
    {
        fprintf(stderr, "Couldn't open or create file 'proj2.out'\n");
        exit(1);
    }
    fclose(fp);

    // initializing semaphores
    oxy_queue = sem_open("/xproko47.ios.projekt2-oxy_queue", O_CREAT | O_EXCL, 0666, 0);
    hydro_queue = sem_open("/xproko47.ios.projekt2-hydro_queue", O_CREAT | O_EXCL, 0666, 0);
    atom_mutex = sem_open("/xproko47.ios.projekt2-atom_mutex", O_CREAT | O_EXCL, 0666, 1);
    action_mutex = sem_open("/xproko47.ios.projekt2-action_mutex", O_CREAT | O_EXCL, 0666, 1);

    // initializing barrier
    barrier_mutex = sem_open("/xproko47.ios.projekt2-barrier_mutex", O_CREAT | O_EXCL, 0666, 1);
    turnstile1 = sem_open("/xproko47.ios.projekt2-turnstile1", O_CREAT | O_EXCL, 0666, 0);
    turnstile2 = sem_open("/xproko47.ios.projekt2-turnstile2", O_CREAT | O_EXCL, 0666, 1);

    // checking whether initialization of semaphors failed
    if( oxy_queue       == SEM_FAILED ||
        hydro_queue     == SEM_FAILED ||
        atom_mutex      == SEM_FAILED ||
        action_mutex    == SEM_FAILED ||
        barrier_mutex   == SEM_FAILED ||
        turnstile1      == SEM_FAILED ||
        turnstile2      == SEM_FAILED)
    {
        fprintf(stderr, "Initialization of semaphors failed!\n");
        cleanup();
        exit(1);
    }

    // mapping shared memory
    oxy_count = map(oxy_count);
    *oxy_count = 0;

    hydro_count = map(hydro_count);
    *hydro_count = 0;

    oxy_done = map(oxy_done);
    *oxy_done = 0;

    hydro_done = map(hydro_done);
    *hydro_done = 0;

    barrier_count = map(barrier_count);
    *barrier_count = 0;

    action_count = map(action_count);
    *action_count = 1;

    m_count = map(m_count);
    *m_count = 0;

    // checking whether mapping failed
    if( oxy_count       == MAP_FAILED ||
        hydro_count     == MAP_FAILED ||
        oxy_count       == MAP_FAILED ||
        hydro_count     == MAP_FAILED ||
        barrier_count   == MAP_FAILED ||
        action_count    == MAP_FAILED ||
        m_count         == MAP_FAILED)
    {
        fprintf(stderr, "Mapping failed!\n");
        cleanup();
        exit(1);
    }
}

// barrier that holds, until 3 processes have gathered
void barrier()
{
    sem_wait(barrier_mutex);
        *barrier_count += 1;
        if (*barrier_count == 3)
        {
            sem_wait(turnstile2);
            sem_post(turnstile1);
        }
    sem_post(barrier_mutex);

    sem_wait(turnstile1);
    sem_post(turnstile1);

    sem_wait(barrier_mutex);
        *barrier_count -= 1;
        if (*barrier_count == 0)
        {
            sem_wait(turnstile1);
            sem_post(turnstile2);
        }
    sem_post(barrier_mutex);

    sem_wait(turnstile2);
    sem_post(turnstile2);
}

// function for all the oxygen processes
void oxygen(int idO, int o_will_be_done)
{
    // printing status
    sem_wait(action_mutex);
    fp = fopen("proj2.out", "a");
    if(fp == NULL)
    {
        fprintf(stderr, "Couldn't open or create file 'proj2.out'\n");
        exit(1);
    }
    fprintf(fp, "%d: O %d: started\n", (*action_count)++, idO);
    fclose(fp);
    sem_post(action_mutex);
    // end of printing status

    usleep(1000 * (rand() % (nums[ti] + 1)));

    // printing status
    sem_wait(action_mutex);
    fp = fopen("proj2.out", "a");
    if(fp == NULL)
    {
        fprintf(stderr, "Couldn't open or create file 'proj2.out'\n");
        exit(1);
    }
    fprintf(fp, "%d: O %d: going to queue\n", (*action_count)++, idO);
    fclose(fp);
    sem_post(action_mutex);
    // end of printing status

    sem_wait(atom_mutex);

    *oxy_count += 1;
    if(*oxy_done < o_will_be_done) //can pass -> will make it into molecule
    {
        *oxy_done += 1;
    }
    else    // cannot pass -> would't make it into molecule
    {
        // printing status
        sem_wait(action_mutex);
        fp = fopen("proj2.out", "a");
        if(fp == NULL)
        {
            fprintf(stderr, "Couldn't open or create file 'proj2.out'\n");
            exit(1);
        }
        fprintf(fp, "%d: O %d: not enough H\n", (*action_count)++, idO);
        fclose(fp);
        sem_post(action_mutex);
        // end of printing status

        sem_post(atom_mutex);
        exit(0);
    }

    if (*hydro_count >= 2)
    {
        sem_post(hydro_queue);
        sem_post(hydro_queue);
        *hydro_count -= 2;
        sem_post(oxy_queue);
        *oxy_count -= 1;

        *m_count += 1;
    }
    else
    {
        sem_post(atom_mutex);
    }
    sem_wait(oxy_queue);

    // printing status
    sem_wait(action_mutex);
    fp = fopen("proj2.out", "a");
    if(fp == NULL)
    {
        fprintf(stderr, "Couldn't open or create file 'proj2.out'\n");
        exit(1);
    }
    fprintf(fp, "%d: O %d: creating molecule %d\n", (*action_count)++, idO, *m_count);
    fclose(fp);
    sem_post(action_mutex);
    // end of printing status

    usleep(1000 * (rand() % (nums[tb] + 1)));

    barrier();

    // printing status
    sem_wait(action_mutex);
    fp = fopen("proj2.out", "a");
    if(fp == NULL)
    {
        fprintf(stderr, "Couldn't open or create file 'proj2.out'\n");
        exit(1);
    }
    fprintf(fp, "%d: O %d: molecule %d created\n", (*action_count)++, idO, *m_count);
    fclose(fp);
    sem_post(action_mutex);
    // end of printing status

    sem_post(atom_mutex);

    exit(0);
}

// function for all the hydrogen processes
void hydrogen(int idH, int o_will_be_done, int h_will_be_done)
{
    // printing status
    sem_wait(action_mutex);
    fp = fopen("proj2.out", "a");
    if(fp == NULL)
    {
        fprintf(stderr, "Couldn't open or create file 'proj2.out'\n");
        exit(1);
    }
    fprintf(fp, "%d: H %d: started\n", (*action_count)++, idH);
    fclose(fp);
    sem_post(action_mutex);
    // end of printing status

    usleep(1000 * (rand() % (nums[ti] + 1)));

    // printing status
    sem_wait(action_mutex);
    fp = fopen("proj2.out", "a");
    if(fp == NULL)
    {
        fprintf(stderr, "Couldn't open or create file 'proj2.out'\n");
        exit(1);
    }
    fprintf(fp, "%d: H %d: going to queue\n", (*action_count)++, idH);
    fclose(fp);
    sem_post(action_mutex);
    // end of printing status

    sem_wait(atom_mutex);

    *hydro_count += 1;

    //can pass -> will make it into molecule
    if(*oxy_done < o_will_be_done || *hydro_done < h_will_be_done)
    {
        *hydro_done += 1;
    }
    else    // cannot pass -> would't make it into molecule
    {
        // printing status
        sem_wait(action_mutex);
        fp = fopen("proj2.out", "a");
        if(fp == NULL)
        {
            fprintf(stderr, "Couldn't open or create file 'proj2.out'\n");
            exit(1);
        }
        fprintf(fp, "%d: H %d: not enough O or H\n", (*action_count)++, idH);
        fclose(fp);
        sem_post(action_mutex);
        // end of printing status

        sem_post(atom_mutex);
        exit(0);
    }
    
    if (*hydro_count >= 2 && *oxy_count >= 1)
    {
        sem_post(hydro_queue);
        sem_post(hydro_queue);
        *hydro_count -= 2;
        sem_post(oxy_queue);
        *oxy_count -= 1;

        *m_count += 1;
    }
    else
    {
        sem_post(atom_mutex);
    }
    sem_wait(hydro_queue);

    // printing status
    sem_wait(action_mutex);
    fp = fopen("proj2.out", "a");
    if(fp == NULL)
    {
        fprintf(stderr, "Couldn't open or create file 'proj2.out'\n");
        exit(1);
    }
    fprintf(fp, "%d: H %d: creating molecule %d\n", (*action_count)++, idH, *m_count);
    fclose(fp);
    sem_post(action_mutex);
    // end of printing status

    barrier();

    // printing status
    sem_wait(action_mutex);
    fp = fopen("proj2.out", "a");
    if(fp == NULL)
    {
        fprintf(stderr, "Couldn't open or create file 'proj2.out'\n");
        exit(1);
    }
    fprintf(fp, "%d: H %d: molecule %d created\n", (*action_count)++,idH, *m_count);
    fclose(fp);
    sem_post(action_mutex);
    // end of printing status

    exit(0);
}

// function checks validity of arguments and return them in the nums array
void argument_check(int argc, char* argv[])
{
    if(argc != 5)
    {
        fprintf(stderr, "Invalid argument count!");
        exit(1);
    }

    bool statement;
    for(int i = 0; i < 4; i++)
    {
        nums[i] = atoi_with_check(argv[i + 1]);
        if(i < 2)
        {
            statement = nums[i] <= 0;
        }
        else
        {
            statement = nums[i] < 0 || nums[i] > 1000;
        }

        if(statement)
        {
            fprintf(stderr, "Invalid %d. argument!\n", i + 1);
            exit(1);
        }
    }
}

int main(int argc, char* argv[])
{
    argument_check(argc, argv);
    
    init();

    int h_will_be_done = 0;
    int o_will_be_done = 0;

    // calculating how many Os and Hs will make it into molecule
    for(int i = 0; i < nums[no]; i++)
    {
        if(nums[nh] - h_will_be_done > 1)
        {
            h_will_be_done += 2;
            o_will_be_done++;
        }
    }

    // creating processes
    pid_t pr;
    for(int i = 0; i < nums[no]; i++)
    {
        pr = fork();
        if(pr == -1)
        {
            fprintf(stderr, "Fork failed!\n");
            cleanup();
            exit(1);
        }
        else if(pr == 0)
        {
            oxygen(i, o_will_be_done);
        }
    }
    for(int i = 0; i < nums[nh]; i++)
    {
        pr = fork();
        if(pr == -1)
        {
            fprintf(stderr, "Fork failed!\n");
            cleanup();
            exit(1);
        }
        else if(pr == 0)
        {
            hydrogen(i, o_will_be_done, h_will_be_done);
        }
    }
    for(int i = 0; i < nums[no] + nums[nh]; i++)
    {
        wait(NULL); // ending zombie processes
    }
    
    cleanup();

    return 0;
}