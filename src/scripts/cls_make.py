"""
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
"""

import sys
import os
import string
import datetime

valid_chars = string.ascii_letters + string.digits + "_"

COPYRIGHT_HOLDER = "William Redding"
COPYRIGHT_YEAR = datetime.datetime.now().year
LICENCE_TEXT = f"""MIT License

Copyright (c) {COPYRIGHT_YEAR} {COPYRIGHT_HOLDER}

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE."""
LICENSE_NAME = "MIT"

cpp_reserved_keywords = [
	"alignas",
	"alignof",
	"and",
	"and_eq",
	"asm",
	"atomic_cancel",
	"atomic_commit",
	"atomic_noexcept",
	"auto",
	"bitand",
	"bitor",
	"bool",
	"break",
	"case",
	"catch",
	"char",
	"char8_t",
	"char16_t",
	"char32_t",
	"class",
	"compl",
	"concept",
	"const",
	"consteval",
	"constexpr",
	"constinit",
	"const_cast",
	"continue",
	"co_await",
	"co_return",
	"co_yield",
	"decltype",
	"default",
	"delete",
	"do",
	"double",
	"dynamic_cast",
	"else",
	"enum",
	"explicit",
	"export",
	"extern",
	"false",
	"float",
	"for",
	"friend",
	"goto",
	"if",
	"inline",
	"int",
	"long",
	"mutable",
	"namespace",
	"new",
	"noexcept",
	"not",
	"not_eq",
	"nullptr",
	"operator",
	"or",
	"or_eq",
	"private",
	"protected",
	"public",
	"reflexpr",
	"register",
	"reinterpret_cast",
	"requires",
	"return",
	"short",
	"signed",
	"sizeof",
	"static",
	"static_assert",
	"static_cast",
	"struct",
	"switch",
	"synchronized"
	"template",
	"this",
	"thread_local",
	"throw",
	"true",
	"try",
	"typedef",
	"typeid",
	"typename",
	"union",
	"unsigned",
	"using",
	"virtual",
	"void",
	"volatile",
	"wchar_t",
	"while",
	"xor",
	"xor_eq",
]

def cls_make(path: str):
	path_no_ext = os.path.splitext(path)[0]
	cls_name = os.path.basename(path_no_ext)
	include_guard_macro = cls_name.upper() + "_H"

	with open(path + ".hpp", "w+") as h:
		h.write(
f"""/*
Copyright (C) {COPYRIGHT_YEAR} {COPYRIGHT_HOLDER} - All Rights Reserved
LICENSE: {LICENSE_NAME}
*/

#ifndef {include_guard_macro}
#define {include_guard_macro}

namespace engine {{
	class {cls_name} {{

	}};
}}

#endif // !{include_guard_macro}

/*
{LICENCE_TEXT}
*/"""
		)

	with open(path + ".cpp", "w+") as cpp:
		cpp.write(
f"""/*
Copyright (C) {COPYRIGHT_YEAR} {COPYRIGHT_HOLDER} - All Rights Reserved
LICENSE: {LICENSE_NAME}
*/

#include \"{cls_name}.hpp\"

/*
{LICENCE_TEXT}
*/"""
		)

if __name__ == "__main__":
	if len(sys.argv) > 1:
		cls_path = sys.argv[1]
	else:
		cls_path = input("Enter class path: ").strip()

	BASE_DIR = os.path.join( os.path.dirname( __file__ ), '..' )
	cls_path = os.path.join(BASE_DIR, cls_path)
	cls_name = os.path.basename(cls_path)

	if not cls_name:
		print("No class name provided")
		sys.exit(0)

	if cls_name in cpp_reserved_keywords:
		print("Class name cannot be a reserved words")
		sys.exit(0)

	if cls_name[0] in string.digits:
		print("Class name cannot start with digit")
		sys.exit(0)

	if cls_name in cpp_reserved_keywords:
		print("Class name cannot be a C++ reserved keyword")
		sys.exit(0)

	invalid_chars = set([char for char in cls_name if char not in valid_chars])

	if invalid_chars:
		print("Class name contains invalid chars:", invalid_chars)
		sys.exit(0)

	cls_make(cls_path)

"""
MIT License

Copyright (c) 2023 William Redding

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
"""