#ifndef UTime_Included
#define UTime_Included

#ifdef __cplusplus
extern "C" {
#endif

struct utimbuf
{
 time_t actime;
 time_t modtime;
};

int utime(const char *file, const struct utimbuf *time);

#ifdef __cplusplus
}
#endif

#endif /* UTime_Included */
