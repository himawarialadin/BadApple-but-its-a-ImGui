#pragma once
#include <string>
class ISandbox;

class Framework
{
public:
	static void Run(ISandbox* sandBox, std::wstring_view title = L"");
	static void Exit();

};
