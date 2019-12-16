
#ifndef PALLETTE_CURRENTTHREAD_H
#define PALLETTE_CURRENTTHREAD_H

#include <stdint.h>

namespace pallette
{
    namespace current_thread
    {
        // internal
        extern __thread int tCachedTid;
        extern __thread const char* tThreadName;
        void cacheTid();

        inline int tid()
        {
            if (__builtin_expect(tCachedTid == 0, 0))
            {
                cacheTid();
            }
            return tCachedTid;
        }

        inline const char* name()
        {
            return tThreadName;
        }

        bool isMainThread();
    }
}

#endif
