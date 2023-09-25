#ifndef FFL_RESULT_H_
#define FFL_RESULT_H_

#include <nn/ffl/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum FFLResult
{
    FFL_RESULT_OK                       =  0,
    FFL_RESULT_ERROR                    =  1,
  //FFL_RESULT_                         =  2,
  //FFL_RESULT_                         =  3,
    FFL_RESULT_MANAGER_NOT_CONSTRUCT    =  4,   // I think
  //FFL_RESULT_                         =  5,
  //FFL_RESULT_                         =  6,
  //FFL_RESULT_                         =  7,
  //FFL_RESULT_                         =  8,
  //FFL_RESULT_                         =  9,
  //FFL_RESULT_                         = 10,
  //FFL_RESULT_                         = 11,
    FFL_RESULT_OUT_OF_MEMORY            = 12,   // I think?
  //FFL_RESULT_                         = 13,
  //FFL_RESULT_                         = 14,
  //FFL_RESULT_                         = 15,
  //FFL_RESULT_                         = 16,
  //FFL_RESULT_                         = 17,
    FFL_RESULT_FS_ERROR                 = 18,
    FFL_RESULT_FS_NOT_FOUND             = 19    // I... think?
}
FFLResult;

#ifdef __cplusplus
}
#endif

#endif // FFL_RESULT_H_
