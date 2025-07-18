#pragma once

class IWindow
{
public:
	IWindow() = default;
	virtual ~IWindow() = default;

public:
	virtual void Show() = 0;

};