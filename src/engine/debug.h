#pragma once
#include <iostream>

#ifdef _DEBUG
#define LB_ASSERT(condition, msg)                                               \
    do                                                                          \
    {                                                                           \
        if (!(condition))                                                       \
        {                                                                       \
            std::cout << "Error in '" << __FILE__ << "' at line #" << __LINE__; \
            std::cout << "\n  --> " << msg << "\n";                             \
            __debugbreak();                                                     \
        }                                                                       \
    } while (0)
#else
#define LB_ASSERT(condition, msg)                   \
    do                                              \
    {                                               \
        if (!(condition))                           \
        {                                           \
            std::cout << "\n  --> " << msg << "\n"; \
        }                                           \
    } while (0)

#endif // _DEBUG