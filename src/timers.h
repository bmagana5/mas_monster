#ifndef TIMER_H
#define TIMER_H

extern double timeDiff(struct timespec *, struct timespec *);
extern void timeCopy(struct timespec *, struct timespec *);
extern void recordTime(struct timespec *);

#endif
