/* rtc.c
Copyright 2021 Carl John Kugler III

Licensed under the Apache License, Version 2.0 (the License); you may not use 
this file except in compliance with the License. You may obtain a copy of the 
License at

   http://www.apache.org/licenses/LICENSE-2.0 
Unless required by applicable law or agreed to in writing, software distributed 
under the License is distributed on an AS IS BASIS, WITHOUT WARRANTIES OR 
CONDITIONS OF ANY KIND, either express or implied. See the License for the 
specific language governing permissions and limitations under the License.
*/
#include <time.h>

#include "ff.h"
#include "rtc.h"

static time_t epochtime = 0;

time_t time(time_t *pxTime) {
    if (pxTime) {
        *pxTime = epochtime;
    }
    return epochtime;
}

void time_init(void) {
    epochtime = 0;
}

// Called by FatFs:
DWORD get_fattime(void) {
    return 0;
}
