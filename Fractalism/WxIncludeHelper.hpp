#ifndef _WX_INCLUDE_HELPER_HPP_
#define _WX_INCLUDE_HELPER_HPP_

// Prevent errors from the 'min' and 'max' macros being declared in Windows.h
#define NOMINMAX
#pragma warning(push)
#pragma warning(disable : 4996)
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#if !defined(WX_PRECOMP)
#include <wx/wx.h>
#endif
#pragma warning(pop)
#undef NOMINMAX

#endif // !_WX_INCLUDE_HELPER_HPP_