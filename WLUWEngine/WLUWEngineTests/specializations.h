#pragma once
#include "CppUnitTest.h"
#include "Vector2.h"
namespace Microsoft
{
    namespace VisualStudio
    {
        namespace CppUnitTestFramework
        {
            template<> inline std::wstring ToString<WLUW::Vector2>(const class WLUW::Vector2& v) { return std::wstring(L"Vector2"); }
        }
    }
}