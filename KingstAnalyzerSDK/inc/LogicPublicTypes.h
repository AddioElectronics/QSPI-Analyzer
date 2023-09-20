#ifndef LOGICPUBLICTYPES
#define LOGICPUBLICTYPES

#ifndef WIN32
	#define __cdecl
#endif

#ifdef WIN32
	#define LOGICAPI __declspec(dllexport)
#else
	#define LOGICAPI __attribute__ ((visibility("default")))
#endif

#ifdef WIN32
	#define ANALYZER_EXPORT __declspec(dllexport)
#else
	#define ANALYZER_EXPORT __attribute__ ((visibility("default")))
#endif

typedef signed char S8;         /*  8 bit signed   */
typedef unsigned char U8;       /*  8 bit unsigned */
typedef short S16;              /* 16 bit signed   */
typedef unsigned short U16;     /* 16 bit unsigned */
typedef int S32;                /* 32 bit signed   */
typedef unsigned int U32;       /* 32 bit unsigned */
typedef long long S64;          /* 64 bit signed   */
typedef unsigned long long U64; /* 64 bit unsigned */

#ifndef NULL
#define NULL 0
#endif

enum DisplayBase { Binary, Decimal, Hexadecimal, ASCII, AsciiHex };
enum BitState { BIT_LOW, BIT_HIGH };

#define Toggle(x) (x == BIT_LOW ? BIT_HIGH : BIT_LOW)
#define Invert(x) (x == BIT_LOW ? BIT_HIGH : BIT_LOW)

#define UNDEFINED_CHANNEL Channel(0xFFFFFFFFFFFFFFFFull, 0xFFFFFFFF)

//Note this class can not add, remove or reorder member variables without breaking binary compatibilty
//This function can add non-virtual functions as long as they are listed after existing functions.
class LOGICAPI Channel
{
public:
    Channel();
    Channel(const Channel &channel);
    Channel(U64 device_id, U32 channel_index);
    ~Channel();

    Channel &operator=(const Channel &channel);
    bool operator==(const Channel &channel) const;
    bool operator!=(const Channel &channel) const;
    bool operator>(const Channel &channel) const;
    bool operator<(const Channel &channel) const;

    U64 mDeviceId;
    U32 mChannelIndex;
};

#include <memory> //required by some compilers for std::auto_ptr

#ifdef _MSC_VER
#define snprintf _snprintf_s
#define vsnprintf vsprintf_s
#endif

#endif //LOGICPUBLICTYPES
