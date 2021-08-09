#ifndef APPLICATION_IMPLDETAILS_IMPLAPI_ERRORS_HPP_
#define APPLICATION_IMPLDETAILS_IMPLAPI_ERRORS_HPP_
namespace impl {
enum class KernelQueueErrorStatus : unsigned char {

};

enum class ModuleInitErrorStatus : unsigned char {

};

enum class ThreadResourceErrorStatus : unsigned char {
	kMin, // minimal value of the enumeration
	kOk = kMin, // operation on the resource was performed by the caller thread
	kBusy, // the resource belongs to other threads
	kMax = kBusy // maximum value of the enumeration
};

}
#endif // APPLICATION_IMPLDETAILS_IMPLAPI_ERRORS_HPP_
