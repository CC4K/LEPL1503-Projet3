#ifndef PORTABLE_SEMAPHORE_H
#define PORTABLE_SEMAPHORE_H

#include <semaphore.h>
#include <stdint.h>

static inline sem_t *
my_sem_init_with_name(char *name, uint32_t value)
{
#ifdef __APPLE__
    sem_unlink(name);
    return sem_open(name, O_CREAT, 0644, value);
#else
    sem_t *sem = malloc(sizeof(sem_t));
    if (!sem)
    {
        return NULL;
    }
    if (sem_init(sem, 0, value) != 0)
    {
        free(sem);
        return NULL;
    }
    return sem;
#endif /* __APPLE__ */
}

static inline sem_t *
my_sem_init(uint32_t value)
{
    return my_sem_init_with_name("/tmp/myPortableSem", value);
}

static inline int
my_sem_destroy(sem_t *sem)
{
#ifdef __APPLE__
    return sem_close(sem);
#else
    int err = sem_destroy(sem);
    free(sem);
    return err;
#endif /* __APPLE__ */
}

// Define to hide the portable semaphore
#define sem_init(value) my_sem_init(value)
#define sem_destroy(sem) my_sem_destroy(sem)
// sem_wait and sem_post are equivalent for OSX and Linux => use default function calls

#endif /* PORTABLE_SEMAPHORE_H */
