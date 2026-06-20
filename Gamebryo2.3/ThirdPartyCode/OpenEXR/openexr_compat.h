//------------------------------------------------------------------------------
// openexr_compat.h
//
// Force-included by IlmImf.vcxproj. OpenEXR 1.2.2's ImfAttribute.cpp uses
// std::binary_function but never #include<functional>; it relies on transitive
// inclusion through <map>, which on modern MSVC STL no longer brings it in.
// Pulling <functional> in via this header makes the typedef visible without
// modifying the OpenEXR sources.
//
// std::binary_function/unary_function themselves are still provided by MSVC
// when _HAS_AUTO_PTR_ETC=1 is defined (set in IlmImf.vcxproj's PreprocessorDefinitions).
//------------------------------------------------------------------------------
#pragma once

#if defined(_MSC_VER)
#include <functional>
#endif
