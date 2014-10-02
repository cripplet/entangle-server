#ifndef _ENTANGLE_DOPT_VOID_HOOK_H
#define _ENTANGLE_DOPT_VOID_HOOK_H

#include <cstddef>

namespace entangle {
	class OTVoidHook {
		public:
			virtual void i(size_t pos, char c) = 0;
			virtual void e(size_t pos) = 0;
	};
}

#endif
