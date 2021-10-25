#ifndef APPLICATION_IMPLDETAILS_IMPLAPI_ERRORS_HPP_
#define APPLICATION_IMPLDETAILS_IMPLAPI_ERRORS_HPP_
namespace impl {
	// Error while module initialization
enum class ModuleInitErrorStatus : unsigned char {

};

enum class ThreadResourceErrorStatus : unsigned char {
	kMin, // minimal value of the enumeration
	kOk = kMin, // operation on the resource was performed by the caller thread
	kBusy, // the resource belongs to other threads
	kMax = kBusy // maximum value of the enumeration
};

// Thread error code which must be handled by kernel
enum class ThreadError : unsigned char {

};

// Module error code which must be handled by kernel
enum class ModuleError : unsigned char {

};
}
#endif // APPLICATION_IMPLDETAILS_IMPLAPI_ERRORS_HPP_
