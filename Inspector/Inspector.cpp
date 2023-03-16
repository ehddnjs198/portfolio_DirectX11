#include "stdafx.h"
#include "Inspector/Inspector.h"

#include "Graphic/Graphic.h"

#include "imGui/imgui.h"
#include "imGui/imgui_impl_dx11.h"
#include "imGui/imgui_impl_win32.h"
#include "imGuizmo/ImGuizmo.h"

#include "cTestBox.h"

#include "Canvas/cCanvas.h"
#include "Canvas/Image/cImage.h"
#include "Canvas/UI/Button/cUIButton.h"
#include "Canvas/UI/cUIBase.h"
#include "Grid/cGrid.h"

#include "MeshRenderer.h"
#include "SkinnedMesh.h"
#include "MeshAnimator.h"

#include "Setting.h"

#include "Ray/cRay.h"




static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }

void Frustum(float left, float right, float bottom, float top, float znear, float zfar, float* m16)
{
	float temp, temp2, temp3, temp4;
	temp = 2.0f * znear;
	temp2 = right - left;
	temp3 = top - bottom;
	temp4 = zfar - znear;
	m16[0] = temp / temp2;
	m16[1] = 0.0;
	m16[2] = 0.0;
	m16[3] = 0.0;
	m16[4] = 0.0;
	m16[5] = temp / temp3;
	m16[6] = 0.0;
	m16[7] = 0.0;
	m16[8] = (right + left) / temp2;
	m16[9] = (top + bottom) / temp3;
	m16[10] = (-zfar - znear) / temp4;
	m16[11] = -1.0f;
	m16[12] = 0.0;
	m16[13] = 0.0;
	m16[14] = (-temp * zfar) / temp4;
	m16[15] = 0.0;
}

void Perspective(float fovyInDegrees, float aspectRatio, float znear, float zfar, float* m16)
{
	float ymax, xmax;
	ymax = znear * tanf(fovyInDegrees * 3.141592f / 180.0f);
	xmax = ymax * aspectRatio;
	Frustum(-xmax, xmax, -ymax, ymax, znear, zfar, m16);
}

void Cross(const float* a, const float* b, float* r)
{
	r[0] = a[1] * b[2] - a[2] * b[1];
	r[1] = a[2] * b[0] - a[0] * b[2];
	r[2] = a[0] * b[1] - a[1] * b[0];
}

float Dot(const float* a, const float* b)
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

void Normalize(const float* a, float* r)
{
	float il = 1.f / (sqrtf(Dot(a, a)) + FLT_EPSILON);
	r[0] = a[0] * il;
	r[1] = a[1] * il;
	r[2] = a[2] * il;
}

void LookAt(const float* eye, const float* at, const float* up, float* m16)
{
	float X[3], Y[3], Z[3], tmp[3];

	tmp[0] = eye[0] - at[0];
	tmp[1] = eye[1] - at[1];
	tmp[2] = eye[2] - at[2];
	//Z.normalize(eye - at);
	Normalize(tmp, Z);
	Normalize(up, Y);
	//Y.normalize(up);

	Cross(Y, Z, tmp);
	//tmp.cross(Y, Z);
	Normalize(tmp, X);
	//X.normalize(tmp);

	Cross(Z, X, tmp);
	//tmp.cross(Z, X);
	Normalize(tmp, Y);
	//Y.normalize(tmp);

	m16[0] = X[0];
	m16[1] = Y[0];
	m16[2] = Z[0];
	m16[3] = 0.0f;
	m16[4] = X[1];
	m16[5] = Y[1];
	m16[6] = Z[1];
	m16[7] = 0.0f;
	m16[8] = X[2];
	m16[9] = Y[2];
	m16[10] = Z[2];
	m16[11] = 0.0f;
	m16[12] = -Dot(X, eye);
	m16[13] = -Dot(Y, eye);
	m16[14] = -Dot(Z, eye);
	m16[15] = 1.0f;
}

void OrthoGraphic(const float l, float r, float b, const float t, float zn, const float zf, float* m16)
{
	m16[0] = 2 / (r - l);
	m16[1] = 0.0f;
	m16[2] = 0.0f;
	m16[3] = 0.0f;
	m16[4] = 0.0f;
	m16[5] = 2 / (t - b);
	m16[6] = 0.0f;
	m16[7] = 0.0f;
	m16[8] = 0.0f;
	m16[9] = 0.0f;
	m16[10] = 1.0f / (zf - zn);
	m16[11] = 0.0f;
	m16[12] = (l + r) / (l - r);
	m16[13] = (t + b) / (b - t);
	m16[14] = zn / (zn - zf);
	m16[15] = 1.0f;
}

void EditTransform(const float* cameraView, float* cameraProjection, float* matrix)
{
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
	static bool useSnap = false;
	static float snap[3] = { 1.f, 1.f, 1.f };
	static float bounds[] = { -50.f, -50.f, -50.f, 50.f, 50.f, 50.f };
	static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
	static bool boundSizing = false;
	static bool boundSizingSnap = false;

	if (KEYMANAGER->isOnceKeyDown('Q') || BOX->clickRed)
	{
		BOX->clickRed = false;

		BOX->getKeyboardEvent()[0] = false;
		BOX->getKeyboardEvent()[1] = true;
		BOX->getKeyboardEvent()[2] = false;
		BOX->getKeyboardEvent()[3] = false;
		BOX->getKeyboardEvent()[4] = false;
	}

	if (KEYMANAGER->isOnceKeyDown('W') || BOX->clickBlue)
	{
		BOX->clickBlue = false;
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		BOX->getKeyboardEvent()[0] = false;
		BOX->getKeyboardEvent()[1] = false;
		BOX->getKeyboardEvent()[2] = true;
		BOX->getKeyboardEvent()[3] = false;
		BOX->getKeyboardEvent()[4] = false;

	}
	if (KEYMANAGER->isOnceKeyDown('E') || BOX->clickBlack)
	{
		BOX->clickBlack = false;
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
		BOX->getKeyboardEvent()[0] = false;
		BOX->getKeyboardEvent()[1] = false;
		BOX->getKeyboardEvent()[2] = false;
		BOX->getKeyboardEvent()[3] = true;
		BOX->getKeyboardEvent()[4] = false;

	}
	if (KEYMANAGER->isOnceKeyDown('R') || BOX->clickGreen) // r Key
	{
		BOX->clickGreen = false;
		mCurrentGizmoOperation = ImGuizmo::SCALE;
		BOX->getKeyboardEvent()[0] = false;
		BOX->getKeyboardEvent()[1] = false;
		BOX->getKeyboardEvent()[2] = false;
		BOX->getKeyboardEvent()[3] = false;
		BOX->getKeyboardEvent()[4] = true;

	}
	if (ImGui::Button("Reset"))
	{
		objectMatrix[0] = 1, objectMatrix[1] = 0, objectMatrix[2] = 0, objectMatrix[3] = 0;
		objectMatrix[4] = 1, objectMatrix[5] = 0, objectMatrix[6] = 0, objectMatrix[7] = 0;
		objectMatrix[8] = 1, objectMatrix[9] = 0, objectMatrix[10] = 0, objectMatrix[11] = 0;
		objectMatrix[12] = 1, objectMatrix[13] = 0, objectMatrix[14] = 0, objectMatrix[15] = 0;

	}
	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
	ImGui::InputFloat3("Tr", matrixTranslation, 3);
	ImGui::InputFloat3("Rt", matrixRotation, 3);
	ImGui::InputFloat3("Sc", matrixScale, 3);
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);

	if (mCurrentGizmoOperation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
			mCurrentGizmoMode = ImGuizmo::LOCAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
			mCurrentGizmoMode = ImGuizmo::WORLD;
	}
	if (ImGui::IsKeyPressed(83))
		useSnap = !useSnap;
	ImGui::Checkbox("", &useSnap);
	ImGui::SameLine();

	switch (mCurrentGizmoOperation)
	{
	case ImGuizmo::TRANSLATE:
		ImGui::InputFloat3("Snap", &snap[0]);
		break;
	case ImGuizmo::ROTATE:
		ImGui::InputFloat("Angle Snap", &snap[0]);
		break;
	case ImGuizmo::SCALE:
		ImGui::InputFloat("Scale Snap", &snap[0]);
		break;
	}
	ImGui::Checkbox("Collider Box", &boundSizing);
	if (boundSizing)
	{
		ImGui::PushID(3);
		ImGui::Checkbox("", &boundSizingSnap);
		ImGui::SameLine();
		ImGui::InputFloat3("Snap", boundsSnap);
		ImGui::PopID();
	}

	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, matrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);
}

//
//
// ImSequencer interface
//
//

inline void rotationY(const float angle, float* m16)
{
	float c = cosf(angle);
	float s = sinf(angle);

	m16[0] = c;
	m16[1] = 0.0f;
	m16[2] = -s;
	m16[3] = 0.0f;
	//////////////

	m16[4] = 0.0f;
	m16[5] = 1.f;
	m16[6] = 0.0f;
	m16[7] = 0.0f;
	/////////////

	m16[8] = s;
	m16[9] = 0.0f;
	m16[10] = c;
	m16[11] = 0.0f;
	//////////////

	m16[12] = 0.f;
	m16[13] = 0.f;
	m16[14] = 0.f;
	m16[15] = 1.0f;
}


Inspector::Inspector()
{

	grid = new cGrid(6, 6);

	ctestBox = new cTestBox(D3DXCOLOR(0, 1, 0, 1));

}

Inspector::~Inspector()
{
}

void Inspector::Initialize()
{


	D3DXMatrixIdentity(&m_Mat_World);

	
	SkinnedMesh::GetInstance().LoadMesh("fbx/Boxing.fbx");
	SkinnedMesh::GetInstance().SetTexture("fbx/Boxing.fbm/bear_diffuse.png", 0);
	//m_pSkinnedMesh->SetTexture("fbx/Hip Hop Dancing.fbm/vanguard_diffuse.png", 1);

	m_pMeshRender = new MeshRenderer();
	m_pMeshRender->Init();

	m_pAnimator = new MeshAnimator();
	m_pAnimator->Init();

	m_pCanvas = new cCanvas();
	m_pCanvas->Initialize(
		Setting::GetInstance().GetWindowWidth(),
		Setting::GetInstance().GetWindowHeight(),
		0.3f, 1000.f);

	
	D3DXMatrixIdentity(&m_Mat_Word);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(Setting::GetInstance().GetWindowHandle());
	ImGui_ImplDX11_Init(Graphic::GetInstance().GetDevice(), Graphic::GetInstance().GetDeviceContext());
	ImGui::StyleColorsLight();


	//rotationY(0.f, objectMatrix);

}

void Inspector::Update()
{

	if (BOX->isLoad)
	{

	}

	ImGuiIO& io = ImGui::GetIO();

	if (io.MouseDown[0])
	{
		D3DXVECTOR2 vScreenPos;
		POINT p;
		p.x = io.MousePos.x;
		p.y = io.MousePos.y;


		cRay r;
		//ȭ�� ��ġ�� �̻��ؼ� �׷���.
		//Ŭ���̾�Ʈ�� ��ġ�� ����� �޾Ƽ� �����ϰ� ����. �� Ŭ���̾�Ʈ ��ġ ... ���..
		r.RayAtWorldSpace(p, cameraView, cameraProjection);

		D3DXVECTOR3				m_vPickPos;

		//����Ƽ������ �浹�� �Ǹ� �ش� ���͸����� �÷����� �����Ų��.
		//���ý��� P�� �ְ�, ���̴����� �÷��� �����ϴ� ������� �Ұ��̴�.
		//Ŭ�� -> �ڽ��� �ϸ� �� �ش� ������Ʈ�� ���͸����� ������ �������ش�.
		//�ڽ� �ȿ� ���͸����� �ִ�. �嵥 ���͸��� �����ϸ� �÷���..���̴��� �����ؾߵȴ�.
		//���̴� �ȿ� ���� �÷��� �־����. �װ� ShaderColor_0���� �Ұ���.

		if (r.RayPickingMesh( m_vPickPos))
		{
			BOX->isPicking = true;
		}
		else
		{
			BOX->isPicking = false;
		}
	}


	if (KEYMANAGER->isOnceKeyDown(VK_RETURN))
	{
		system("cls");
	}


	ctestBox->Update(BOX->getMatirix(), cameraView, cameraProjection);

	m_pMeshRender->Update(objectMatrix, cameraView, cameraProjection);
	if (BOX->isAnimation)
	{
		m_pAnimator->Upadate();//�ִϸ��̼� �ϴ� �κ�

	}

	m_pCanvas->Update(identityMatrix, cameraView);


	D3DXMatrixScaling(&m_Mat_Word, 100, 100, 100);
	grid->Update(m_Mat_Word, cameraView, cameraProjection);
}

void Inspector::Render()
{

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO();



	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			if (ImGui::MenuItem("New")) {}
			if (ImGui::MenuItem("Open", "Ctrl+O")) 
			{
				BOX->isOpen = true;
			}
			if (ImGui::MenuItem("Save", "Ctrl+S"))
			{
				BOX->fileSave = true;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Object"))
		{
			if (ImGui::MenuItem("Grid"))
			{
				gridFlag = true;
			}
			if (ImGui::MenuItem("Plane"))
			{
				planeFlag = true;
			}
			if (ImGui::MenuItem("Monster"))
			{
				BOX->isMonster = true;
			}

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Effect"))
		{
			if (ImGui::MenuItem("Wireframe"))
			{
				if (BOX->isWireFrame)
				{
					BOX->isWireFrame = false;
				}
				else
				{
					BOX->isWireFrame = true;
				}
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	////////////////////////////////////////////////////////////�Ŵ���

	if (BOX->isBox)
	{
		ctestBox->Render();

	}


	if (gridFlag)
	{
		grid->Render();
	}
	if (BOX->isMonster)
	{
		m_pMeshRender->Render();

	}

	if (isPerspective)
	{
		Perspective(fov, io.DisplaySize.x / io.DisplaySize.y, 0.1f, 100.f, cameraProjection);
	}
	else
	{
		float viewHeight = viewWidth * io.DisplaySize.y / io.DisplaySize.x;
		OrthoGraphic(-viewWidth, viewWidth, -viewHeight, viewHeight, -viewWidth, viewWidth, cameraProjection);
	}


	if (KEYMANAGER->isOnceKeyDown(VK_RETURN))
	{
		system("cls");
		for (int i = 0; i < 16; i++)
		{
			if (i % 4 == 0)cout << "x =";
			if (i % 4 == 1)cout << "y =";
			if (i % 4 == 2)cout << "z =";


			cout << objectMatrix[i] << endl;
		}
	}

	if (KEYMANAGER->isStayKeyDown(VK_UP))
	{

		cameraView[14] += 10.f;
	}

	if (KEYMANAGER->isStayKeyDown(VK_DOWN))
	{

		cameraView[14] -= 10.f;
	}
	if (KEYMANAGER->isStayKeyDown(VK_LEFT))
	{
		
		cameraView[12] += 10.f;

	}
	if (KEYMANAGER->isStayKeyDown(VK_RIGHT))
	{
		cameraView[12] -= 10.f;

	}
	if (KEYMANAGER->isStayKeyDown('A'))
	{
		cameraView[13] += 10.f;

	}

	if (KEYMANAGER->isStayKeyDown('D'))
	{
		cameraView[13] -= 10.f;

	}


	ImGuizmo::SetOrthographic(!isPerspective);


	ImGuizmo::BeginFrame();

	


	ImGui::Begin("Inspector", &test);
	ImGui::Text("Camera");

	if (BOX->isMonster)
	{
		ImGui::Text("Monster");
	}

	bool viewDirty = false;
	if (ImGui::RadioButton("Perspective", isPerspective)) isPerspective = true;
	ImGui::SameLine();
	if (ImGui::RadioButton("Orthographic", !isPerspective)) isPerspective = false;
	if (isPerspective)
	{
		ImGui::SliderFloat("Fov", &fov, 20.f, 110.f);
	}
	else
	{
		ImGui::SliderFloat("Ortho width", &viewWidth, 1, 1000);
	}
	viewDirty |= ImGui::SliderAngle("Camera X", &cameraView[12]);
	viewDirty |= ImGui::SliderAngle("Camera Y", &cameraView[13]);
	viewDirty |= ImGui::SliderFloat("Distance", &camDistance, 1.f, 10.f);

	if (viewDirty || firstFrame)
	{
		LookAt(eye, at, up, cameraView);
		firstFrame = false;
	}


	//ImGuizmo::DrawCube(cameraView, cameraProjection, objectMatrix);
	
	//ImGuizmo::DrawGrid(cameraView, cameraProjection, identityMatrix, 10.f);
	
	m_pCanvas->Render();

	if (stringTest)
	{

	}
	 
	
	ImGui::Text("X: %f Y: %f", io.MousePos.x, io.MousePos.y);
	ImGui::Separator();
	EditTransform(cameraView, cameraProjection, objectMatrix);
	//EditTransform(cameraView, cameraProjection, boundingMatrix);

	ImGui::End();

	
	ImGui::Begin("Project", &test3);

	if (gridFlag)
	{
		if (ImGui::Button("DeleteGrid"))
		{
			gridFlag = false;
		}
	}
	if (BOX->isMonster)
	{
		if (ImGui::Button("DeleteMonster"))
		{
			BOX->isMonster = false;
		}
	}
	if (BOX->isBox)
	{
		if (ImGui::Button("DeleteColliderBox"))
		{
			BOX->isBox = false;
		}
	}

	ImGui::End();

	ImGui::Begin("Hierarchy", &test3);
	ImGui::Button("MainCamera");

	if (gridFlag)
	{
		ImGui::Button("Grid");
	}

	if (planeFlag)
	{

	}
	ImGui::End();

	char buf[16] = "sdf";
	ImGui::Begin("Console", &test1);

	//ImGui::InputText("ssss", buf, IM_ARRAYSIZE(buf));
	////ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	//
	//if (buf[0] == 't' &&
	//	buf[1] == 'e' &&
	//	buf[2] == 's' &&
	//	buf[3 == 't'])
	//{
	//	stringTest = true;
	//}
	if (ImGui::Button("Clear"))
	{
		isClear = true;
	}
	if (isClear)
	{
		isClear = false;
		Message = "";
	}
	if (BOX->isPicking)
	{
		if(BOX->isMonster)
			Message += "picking is success\n";
	}
	ImGui::Text(Message.c_str());


	ImGui::End();


	ImGui::Render();
		ImGuizmo::ViewManipulate(cameraView, camDistance, ImVec2(io.DisplaySize.x - 128, 0), ImVec2(128, 128), WHITE_BRUSH);


	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

