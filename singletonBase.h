#pragma once
//===============================
// # singletonBase # (싱글톤 패턴)
//===============================

template <class T>
class singletonBase
{
protected:
	//싱글톤 인스턴스 선언
	static T* singleton;

	singletonBase() {}
	~singletonBase() {}
public:
	//싱글톤 가져오기
	static T* getSingleton(void);
	//싱글톤 메모리에서 해제
	void releaseSingleton(void);
};

//싱글톤 초기화
template <typename T>
T* singletonBase<T>::singleton = 0;

//싱글톤 가져오기
template <typename T>
T* singletonBase<T>::getSingleton(void)
{
	//싱글톤이 없으면 새로 생성
	if (!singleton) singleton = new T;

	return singleton;
}

//싱글톤 메모리 해제
template <typename T>
void singletonBase<T>::releaseSingleton(void)
{
	//싱글톤이 있다면 메모리에서 해제
	if (singleton)
	{
		delete singleton;
		singleton = 0;
	}
}