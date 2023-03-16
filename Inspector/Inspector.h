#pragma once


class MeshRenderer;
class SkinnedMesh;
class MeshAnimator;

class Inspector
{
public:
	Inspector();
	~Inspector();
public:
	void Initialize();
	void Update();
	void Render();

private:

    // Camera projection   
    bool isPerspective = false;
    float fov = 27.f;
    float viewWidth = 1000.f; // for orthographic
    float camYAngle = 165.f / 180.f * 3.14159f;
    float camXAngle = 0.f / 180.f * 3.14159f;
    float camDistance = 80.f;




    bool firstFrame = true;

    class cCanvas* m_pCanvas = nullptr;


    D3DXMATRIX m_Mat_Word;

    bool test = false;
    bool test1 = false;
    bool test2 = false;
    bool test3 = false;
    bool test4 = false;

    bool my_tool_active = false;

    bool stringTest = false;

    //카메라
    float up[3] = { 0.f, 1.f, 0.f };
    float at[3] = { 2.f, 0.f, 0.f };
    float eye[3] = { cosf(camYAngle) * cosf(camXAngle) * camDistance + 2.f, sinf(camXAngle) * camDistance, sinf(camYAngle) * cosf(camXAngle) * camDistance };

    class cTestBox* ctestBox = nullptr;
    class cTestBox* ctestBox1 = nullptr;

    class cGrid* grid = nullptr;

    

    ///메뉴판에 있는것들
    bool gridFlag = false;
    bool planeFlag = false;


	D3DXMATRIX m_Mat_World;

	class MeshRenderer* m_pMeshRender = nullptr;
	class MeshAnimator* m_pAnimator = nullptr;
    
	string Message = "";

	bool isClear = false;




};


