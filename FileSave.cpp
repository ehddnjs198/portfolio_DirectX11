#include "stdafx.h"
#include "FileSave.h"

FileSave::FileSave()
{

}

void FileSave::FileWrite()
{

	char buffer[20000] = { 0, };    // 파일을 읽을 때 사용할 임시 공간, 미리 0으로 전부 초기화

	FILE* fp1 = fopen("box.obj", "r");     // hello.txt 파일을 읽기 모드로 열기.
											// 파일 포인터를 반환

	fread(buffer, sizeof(buffer), 1, fp1);   // hello.txt에서 버퍼 크기(20바이트)만큼 1번 값을 읽음

	//printf("%s\n", buffer);    // Hello, world!: 파일의 내용 출력
	string str1 = "";
	string str2 = "";
	string str3 = "";
	string str4 = "";
	string str5 = "";
	string str6 = "";
	string str7 = "";
	string str8 = "";

	for (int i = 0; i < 9; i++)
	{
		str1 += buffer[95 + i];
		str2 += buffer[129 + i];
		str3 += buffer[164 + i];
		str4 += buffer[197 + i];
		str5 += buffer[231 + i];
		str6 += buffer[265 + i];
		str7 += buffer[300 + i];
		str8 += buffer[333 + i];
	}
	float a1 = stof(str1);
	a1 = a1 * BOX->xRatio / 100;
	str1 = to_string(a1);

	a1 = stof(str2);
	a1 = a1 * BOX->xRatio / 100;
	str2 = to_string(a1);

	a1 = stof(str3);
	a1 = a1 * BOX->xRatio / 100;
	str3 = to_string(a1);

	a1 = stof(str4);
	a1 = a1 * BOX->xRatio / 100;
	str4 = to_string(a1);

	a1 = stof(str5);
	a1 = a1 * BOX->xRatio / 100;
	str5 = to_string(a1);

	a1 = stof(str6);
	a1 = a1 * BOX->xRatio / 100;
	str6 = to_string(a1);

	a1 = stof(str7);
	a1 = a1 * BOX->xRatio / 100;
	str7 = to_string(a1);

	a1 = stof(str8);
	a1 = a1 * BOX->xRatio / 100;
	str8 = to_string(a1);



	for (int i = 0; i < 8; i++)
	{
		buffer[95 + i] = str1[i];
		buffer[129 + i] = str2[i];
		buffer[164 + i] = str3[i];
		buffer[197 + i] = str4[i];
		buffer[231 + i] = str5[i];
		buffer[265 + i] = str6[i];
		buffer[300 + i] = str7[i];
		buffer[333 + i] = str8[i];

	}

	////////////////////////////////x

	str1 = "";
	str2 = "";
	str3 = "";
	str4 = "";
	str5 = "";
	str6 = "";
	str7 = "";
	str8 = "";

	for (int i = 0; i < 9; i++)
	{
		str1 += buffer[106 + i];
		str2 += buffer[140 + i];
		str3 += buffer[174 + i];
		str4 += buffer[207 + i];
		str5 += buffer[242 + i];
		str6 += buffer[276 + i];
		str7 += buffer[310 + i];
		str8 += buffer[343 + i];
	}

	a1 = stof(str1);
	a1 = a1 * BOX->yRatio / 100;
	str1 = to_string(a1);

	a1 = stof(str2);
	a1 = a1 * BOX->yRatio / 100;
	str2 = to_string(a1);

	a1 = stof(str3);
	a1 = a1 * BOX->yRatio / 100;
	str3 = to_string(a1);

	a1 = stof(str4);
	a1 = a1 * BOX->yRatio / 100;
	str4 = to_string(a1);

	a1 = stof(str5);
	a1 = a1 * BOX->yRatio / 100;
	str5 = to_string(a1);

	a1 = stof(str6);
	a1 = a1 * BOX->yRatio / 100;
	str6 = to_string(a1);

	a1 = stof(str7);
	a1 = a1 * BOX->yRatio / 100;
	str7 = to_string(a1);

	a1 = stof(str8);
	a1 = a1 * BOX->yRatio / 100;
	str8 = to_string(a1);



	for (int i = 0; i < 8; i++)
	{
		buffer[106 + i] = str1[i];
		buffer[140 + i] = str2[i];
		buffer[174 + i] = str3[i];
		buffer[207 + i] = str4[i];
		buffer[242 + i] = str5[i];
		buffer[276 + i] = str6[i];
		buffer[310 + i] = str7[i];
		buffer[343 + i] = str8[i];
	}

	/////////////////////////////////y
	str1 = "";
	str2 = "";
	str3 = "";
	str4 = "";
	str5 = "";
	str6 = "";
	str7 = "";
	str8 = "";

	for (int i = 0; i < 9; i++)
	{
		str1 += buffer[106 + 10 + i];
		str2 += buffer[140 + 10 + i];
		str3 += buffer[174 + 10 + i];
		str4 += buffer[207 + 10 + i];
		str5 += buffer[242 + 10 + i];
		str6 += buffer[276 + 10 + i];
		str7 += buffer[310 + 10 + i];
		str8 += buffer[343 + 10 + i];
	}

	a1 = stof(str1);
	a1 = a1 * BOX->zRatio / 100;
	str1 = to_string(a1);

	a1 = stof(str2);
	a1 = a1 * BOX->zRatio / 100;
	str2 = to_string(a1);

	a1 = stof(str3);
	a1 = a1 * BOX->zRatio / 100;
	str3 = to_string(a1);

	a1 = stof(str4);
	a1 = a1 * BOX->zRatio / 100;
	str4 = to_string(a1);

	a1 = stof(str5);
	a1 = a1 * BOX->zRatio / 100;
	str5 = to_string(a1);

	a1 = stof(str6);
	a1 = a1 * BOX->zRatio / 100;
	str6 = to_string(a1);

	a1 = stof(str7);
	a1 = a1 * BOX->zRatio / 100;
	str7 = to_string(a1);

	a1 = stof(str8);
	a1 = a1 * BOX->zRatio / 100;
	str8 = to_string(a1);

	for (int i = 0; i < 8; i++)
	{
		buffer[106 + 10 + i] = str1[i];
		buffer[140 + 10 + i] = str2[i];
		buffer[174 + 10 + i] = str3[i];
		buffer[207 + 10 + i] = str4[i];
		buffer[242 + 10 + i] = str5[i];
		buffer[276 + 10 + i] = str6[i];
		buffer[310 + 10 + i] = str7[i];
		buffer[343 + 10 + i] = str8[i];
	}

	cout << str1 << endl;
	cout << str2 << endl;
	cout << str3 << endl;
	cout << str4 << endl;
	cout << str5 << endl;
	cout << str6 << endl;
	cout << str7 << endl;
	cout << str8 << endl;
	cout << "test" << endl;
	///
	cout << buffer[95] << endl;
	cout << buffer[129] << endl;
	cout << buffer[164] << endl;
	cout << buffer[197] << endl;
	cout << buffer[231] << endl;
	cout << buffer[265] << endl;
	cout << buffer[300] << endl;
	cout << buffer[333] << endl;


	fclose(fp1);    // 파일 포인터 닫기



	FILE* fp = fopen("box.txt", "w");    // hello.txt 파일을 쓰기 모드(w)로 열기.
								   // 파일 포인터를 반환

	fwrite(buffer, strlen(buffer), 1, fp);    // strlen으로 문자열의 길이를 구함.
									  // 문자열의 길이만큼 1번 파일에 저장
	fclose(fp);    // 파일 포인터 닫기

	ShellExecute(NULL, "open", "C:\\Users\\Kali\\OneDrive\\바탕 화면\\포폴완성\\정리하기\\DirectXProject\\box.txt", NULL, NULL, SW_SHOW);


	BOX->fileSave = false;
}
