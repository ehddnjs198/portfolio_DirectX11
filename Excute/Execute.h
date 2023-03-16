#pragma once
class Execute final
{
public:
	Execute();
	~Execute();
public:
	void Initialize();
	void Update();
	void Render();

	void Open();

private:

	class Inspector* _inspector = nullptr;

	class FileSave* _fileSave = nullptr;
	
   
};

