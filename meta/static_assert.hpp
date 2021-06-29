#pragma once

namespace meta
{
   template <bool>
   struct CompileTimeCheck
   {
        CompileTimeCheck(...);
   };

   template <>
   struct CompileTimeCheck<true> {};

#define STATIC_ASSERT(expr, msg) \
{\
    class Error_##msg; \
    (void)sizeof(::meta::CompileTimeCheck<expr != 0>(Error_##msg()));\
}

}// namespace meta