#pragma once

#define FRAME_ALLOCATOR_RUNTIME_ERROR_CHECKING
// my C++ version of C code from Game Programming Gems page 92 article "Frame-Based Memory Allocation" by "Steven Ranck"
// variable names kept the same and comments copied faithfully and sometimes added upon, but written as a C++ class with raii behavior.

// It's a memory allocator that works like a stack - and its a 2 in one - two heaps that can be allocated on.
// Memory MUST be freed in the opposite order to which it was obtained
typedef unsigned char u8;
typedef unsigned int uint;

/// <summary>
/// This is a handle that you can request by calling GetFrame before allocating
/// memory. When you call ReleaseFrame passing the handle all memory allocated after the
/// handle was requested will be freed. To be used like a simple token by the consumer,
/// the fields should only be accessed by FrameMemoryAllocator
/// </summary>
struct Frame_t{
	u8* pFrame;
	int nHeapNum;
};

class FrameMemoryAllocator
{
public:
	FrameMemoryAllocator();
	FrameMemoryAllocator(int nSizeInBytes, int nByteAlignment);
	~FrameMemoryAllocator();
private:
	int _nByteAlignment;  // Memory alignment in bytes
	u8* _pMemoryBlock;    // Value returned 
	u8* _apBaseAndCap[2]; // [0]=Base pointer, [1]=Cap pointer
	u8* _apFrame[2];      // [0]=Lower frame pointer, [1]=Upper frame pointer
public:
	/// <summary>
	/// Returns a pointer to the base of the
	/// memory block or returns 0 if there was insufficient memory
	/// </summary>
	/// <param name="nBytes"></param>
	/// <param name="nHeapNum">0=lower, 1=upper</param>
	/// <returns></returns>
	void* AllocFrameMemory(int nBytes, int nHeapNum);

	/// <summary>
	/// Returns a Frame handle which can be used to later release
	/// memory allocated henceforth.
	/// </summary>
	/// <param name="nHeapNum">0=lower, 1=upper</param>
	/// <returns></returns>
	Frame_t GetFrame(int nHeapNum);

	void ReleaseFrame(Frame_t Frame);

	/// <summary>
	/// Must be called exactly once at game initialization time.
	/// </summary>
	/// <param name="nSizeInBytes">must be a multiple of nByteAlignment</param>
	/// <param name="nByteAlignment"> must be a power of 2 </param>
	/// <returns> 0 if successful or 1 if error occured </returns>
	int InitFrameMemorySystem(int nSizeInBytes, int nByteAlignment);	
	void ShutdownFrameMemorySystem();
};

