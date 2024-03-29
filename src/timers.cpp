#include <string.h>
#include <time.h>

const double oobillion = 1.0 / 1e9;

double timeDiff(struct timespec *start, struct timespec *end) {
    return (double)(end->tv_sec - start->tv_sec ) +
	(double)(end->tv_nsec - start->tv_nsec) * oobillion;
}
void timeCopy(struct timespec *dest, struct timespec *source) {
    memcpy(dest, source, sizeof(struct timespec));
}
void recordTime(struct timespec *t) {
	clock_gettime(CLOCK_REALTIME, t);	
}
