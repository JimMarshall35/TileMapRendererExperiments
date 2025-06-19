#include "FrameMemoryAllocator.h"
#include <cassert>

#define ALIGNUP(nAddress, nBytes) ( (((uint)nAddress) + (nBytes) - 1) & (~((nBytes) - 1)) )

inline u8* FrameSystemUnderlyingMalloc(size_t numBytes) {
    // a wrapper in case i ever want to use this code
    // on a different platform or something or allocate memory some other way,
    // new code not from book
    return new u8[numBytes];
}

inline void FrameSystemUnderlyingFree(u8* memory) {
    delete[] memory;
}

FrameMemoryAllocator::FrameMemoryAllocator()
{
}

FrameMemoryAllocator::FrameMemoryAllocator(int nSizeInBytes, int nByteAlignment)
{
    InitFrameMemorySystem(nSizeInBytes, nByteAlignment);
}

FrameMemoryAllocator::~FrameMemoryAllocator()
{
    ShutdownFrameMemorySystem();
}

void* FrameMemoryAllocator::AllocFrameMemory(int nBytes, int nHeapNum)
{
    u8* pMem;

    // First, align the requested size:
    nBytes = ALIGNUP(nBytes, _nByteAlignment);

    // Check for available memory:
    if (_apFrame[0] + nBytes > _apFrame[1]) {
        return 0;
    }

    // Now perform the memory allocation:

    if (nHeapNum) {
        // Allocating from upper heap, down:
        _apFrame[1] -= nBytes;
        pMem = _apFrame[1];
    }
    else {
        // Allocating from lower heap, up:
        pMem = _apFrame[0];
        _apFrame[0] += nBytes;
    }

    return (void*)pMem;
}

Frame_t FrameMemoryAllocator::GetFrame(int nHeapNum)
{
    Frame_t Frame;

    Frame.pFrame = _apFrame[nHeapNum];
    Frame.nHeapNum = nHeapNum;

    return Frame;
}

void FrameMemoryAllocator::ReleaseFrame(Frame_t Frame)
{
#ifdef FRAME_ALLOCATOR_RUNTIME_ERROR_CHECKING
    // Check validity if releasing in lower heap(0):
    assert(Frame.nHeapNum == 1 || (uint)Frame.pFrame <= (uint)_apFrame[0]);
    // Check validity if releasing in upper heap(1):
    assert(Frame.nHeapNum == 0 || (uint)Frame.pFrame >= (uint)_apFrame[1]);
#endif // FRAME_ALLOCATOR_RUNTIME_ERROR_CHECKING

    _apFrame[Frame.nHeapNum] = Frame.pFrame;
}

int FrameMemoryAllocator::InitFrameMemorySystem(int nSizeInBytes, int nByteAlignment)
{
    nSizeInBytes = ALIGNUP(nSizeInBytes, nByteAlignment);

    // first allocate our memory block
    _pMemoryBlock = FrameSystemUnderlyingMalloc(nSizeInBytes + nByteAlignment);
    if (_pMemoryBlock == 0) {
        // not enough memory, return error flag
        return 1;
    }

    _nByteAlignment = nByteAlignment;

    // set up base pointer:
    _apBaseAndCap[0] = (u8*)ALIGNUP(_pMemoryBlock, nByteAlignment);

    // set up cap pointer:
    _apBaseAndCap[1] = (u8*)ALIGNUP(_pMemoryBlock + nSizeInBytes, nByteAlignment);

    // finally, initialize the lower and upper frame pointers:
    _apFrame[0] = _apBaseAndCap[0];
    _apFrame[1] = _apBaseAndCap[1];

    // successful
    return 0;
}

void FrameMemoryAllocator::ShutdownFrameMemorySystem()
{
    if(_pMemoryBlock != nullptr)
        FrameSystemUnderlyingFree(_pMemoryBlock);
}
