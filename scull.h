#ifndef __SCULL_H__
#define __SCULL_H__

struct scull_qset {
    void **data;
    struct scull_qset *next;
};

struct scull_dev {
    struct scull_qset *data;    /* Pointer to first quantum set */
    int quantum;                /* The current quantum size */
    int qset;                   /* The current array size */
    unsinged long size;         /* amount of data stored here */
    unsinged int access_key;    /* used by sculluid and scullpriv */
    struct semaphore sem;       /* mutual exclusion semaphore */
    struct cdev cdev;            /* Char device struct */
};

#define SCULL_MAJOR 0
#define SCULL_MINOR 0
#define SCULL_NR_DEVS 1

#ifndef SCULL_QUANTUM
#define SCULL_QUANTUM 4000
#endif

#ifndef SCULL_QSET
#define SCULL_QSET    1000
#endif

#endif
