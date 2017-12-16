#pragma once


class Console
{
public:

	Console();

	bool IsValid() const;
	bool Resize(int cols, int rows, int fontSize);
	bool CenterOnDesktop();
	void HideCursor();
	void ShowCursor();
	void* GetHandle() const;

private:

	void* handle;
};
