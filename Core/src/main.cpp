/* An example of the minimal Win32 & OpenGL program.  It only works in
16 bit color modes or higher (since it doesn't create a
palette). */

// clang-format off
#include <stdio.h>
#include <windows.h> /* must include this before GL/gl.h */
#include <GL/gl.h> /* OpenGL header file */
#include <GL/glu.h> /* OpenGL utilities header file */
#include "gl_loader.h"
#include <iostream>
#include <string>
#include <memory>

// clang-format on

using namespace std;

int programID;
const int vertexCount = 6;
GLuint vaoID, vboID;

string vertexShader = "#version 430\n\
layout (location = 0) in vec3 position;\n \
out vec2 fragCoord;\n \
\n\
void main()\n \
{\n \
	fragCoord.x = (position.x + 1) * 1 / 2.0;\n \
	fragCoord.y = (position.y + 1) * 1 / 2.0;\n \
	gl_Position = vec4(position, 1);\n \
}";
string fragmentShader = "#version 430\n\
in vec2 fragCoord; \n\
out vec4 fragColor;\n\
float udRoundBox( vec3 p, vec3 b, float r ) {\n\
	return length(max(abs(p) - b, 0.0)) - r;\n\
}\n\
\n\
void main() {\n\
	float iGlobalTime = 0.0; \n\
	vec2 uv = fragCoord.xy;\n\
	vec3 ro = vec3(iGlobalTime * 10.0, 2.0 * sin(iGlobalTime * 0.5), 2.0 * sin(iGlobalTime));\n\
	vec3 rd = normalize(vec3(1.0, uv.y - 0.5, uv.x - 0.5));\n\
	float t = 0.0;\n\
	vec3 color = vec3(0.0);\n\
	float b = (0.5 + 0.5 * sin(iGlobalTime * 10.0)) * 0.2 + 0.3;\n\
\n\
	for (int i = 0; i < 100; i++) {\n\
		vec3 p = ro + rd * t;\n\
		vec3 q = mod(p, 8.0) - 4.0;\n\
		vec3 r = p / 8.0;\n\
		float d = udRoundBox(q, vec3(0.5), 0.3);\n\
\n\
		if (d < 0.01) {\n\
			color = vec3(0.5 + 0.5 * sin(r * 1.3));\n\
\n\
			vec3 lpos = ro + vec3(-1.0, 0, 0); \n\
			float dis = length(lpos - p);\n\
			vec3 invLight = normalize(lpos - p);\n\
\n\
			vec3 normal;\n\
			vec3 ep = vec3(0.01, 0, 0);\n\
			normal.x = udRoundBox(q + ep.xyz, vec3(0.5), 0.3) - udRoundBox(q - ep.xyz, vec3(0.5), 0.3);\n\
			normal.y = udRoundBox(q + ep.yxz, vec3(0.5), 0.3) - udRoundBox(q - ep.yxz, vec3(0.5), 0.3);\n\
			normal.z = udRoundBox(q + ep.yzx, vec3(0.5), 0.3) - udRoundBox(q - ep.yzx, vec3(0.5), 0.3);\n\
			normal = normalize(normal);\n\
\n\
			float diffuse = max(0.0, dot(invLight, normal));\n\
			float s = 10.0;\n\
			float k = max(0.0, dot(rd, reflect(invLight, normal))); \n\
			float spec = pow(k, s);\n\
			float str = 80.0 / (0.1 + 0.1*dis + 0.1*dis*dis);\n\
			color = color * (0.0 + 1.0*diffuse*str) + spec*str;\n\
			break; \n\
		}\n\
		t += d;\n\
	}\n\
	fragColor = vec4(color, 1.0);\n\
	fragColor.rgb = fragColor.rgb / (fragColor.rgb + vec3(1.0));\n\
}";

void setupQuad()
{
	float vertices[] = {
		-1, 1, 0, -1, -1, 0, 1, -1, 0,
		1, -1, 0, 1, 1, 0, -1, 1, 0 };



	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);
	glEnableVertexAttribArray(0);
	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void loadShader()
{


	programID = glCreateProgram();
	int vertID = glCreateShader(GL_VERTEX_SHADER);
	int fragID = glCreateShader(GL_FRAGMENT_SHADER);

	int vertexShaderLength = vertexShader.length();
	auto vertexChar = vertexShader.c_str();
	glShaderSource(vertID, 1, &vertexChar, &vertexShaderLength);

	int fragmentShaderLength = fragmentShader.length();
	auto fragmentChar = fragmentShader.c_str();
	glShaderSource(fragID, 1, &fragmentChar, &fragmentShaderLength);


	glCompileShader(vertID);
	GLint param;
	glGetShaderiv(vertID, GL_COMPILE_STATUS, &param);
	if (param == GL_FALSE) {
		cout << "Failed to compile vertex shader!\n";
		int len;
		auto log = make_unique<GLchar>(1024);
		glGetShaderInfoLog(vertID, 1024, &len, log.get());
		cout << log.get();
	}

	glCompileShader(fragID);

	glGetShaderiv(fragID, GL_COMPILE_STATUS, &param);
	if (param == GL_FALSE) {
		cout << "Failed to compile fragment shader!\n";
		int len;
		char log[1024];
		glGetShaderInfoLog(fragID, 1024, &len, log);
		cout << log;
	}


	glAttachShader(programID, vertID);
	glAttachShader(programID, fragID);
	glLinkProgram(programID);





	//glValidateProgram(program);
	//System.out.println();
	//glGetProgramiv(program, GL_VALIDATE_STATUS, bb2);


	//Delete the shaders
	//glDetachShader(program, vertID);
	//glDetachShader(program, fragID);
	glDeleteShader(vertID);
	glDeleteShader(fragID);
}

void display()
{
	/* rotate a triangle around
	glActiveTexture(GL_TEXTURE0);
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_TRIANGLES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex2i(0, 1);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2i(-1, -1);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex2i(1, -1);
	glEnd();
	glFlush();*/



	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(programID);
	glBindVertexArray(vaoID);
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	glBindVertexArray(0);
	glFlush();
}

LONG WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static PAINTSTRUCT ps;

	switch (uMsg) {
	case WM_PAINT:
		display();
		BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		return 0;

	case WM_SIZE:
		glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
		PostMessage(hWnd, WM_PAINT, 0, 0);
		return 0;

	case WM_CHAR:
		switch (wParam) {
		case 27: /* ESC key */
			PostQuitMessage(0);
			break;
		}
		return 0;

	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HWND CreateOpenGLWindow(char* title, int x, int y, int width, int height,
	BYTE type, DWORD flags)
{
	int pf;
	HDC hDC;
	HWND hWnd;
	WNDCLASS wc;
	PIXELFORMATDESCRIPTOR pfd;
	static HINSTANCE hInstance = 0;

	/* only register the window class once - use hInstance as a flag. */
	if (!hInstance) {
		hInstance = GetModuleHandle(NULL);
		wc.style = CS_OWNDC;
		wc.lpfnWndProc = (WNDPROC)WindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = NULL;
		wc.lpszMenuName = NULL;
		wc.lpszClassName = L"OpenGL";

		if (!RegisterClass(&wc)) {
			MessageBox(NULL, L"RegisterClass() failed:  "
				"Cannot register window class.",
				L"Error", MB_OK);
			return NULL;
		}
	}

	hWnd = CreateWindow(L"OpenGL", L"Hej", WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		x, y, width, height, NULL, NULL, hInstance, NULL);

	if (hWnd == NULL) {
		MessageBox(NULL, L"CreateWindow() failed:  Cannot create a window.",
			L"Error", MB_OK);
		return NULL;
	}

	hDC = GetDC(hWnd);

	/* there is no guarantee that the contents of the stack that become
	the pfd are zeroed, therefore _make sure_ to clear these bits. */
	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | flags;
	pfd.iPixelType = type;
	pfd.cColorBits = 32;

	pf = ChoosePixelFormat(hDC, &pfd);
	if (pf == 0) {
		MessageBox(NULL, L"ChoosePixelFormat() failed:  "
			"Cannot find a suitable pixel format.",
			L"Error", MB_OK);
		return 0;
	}

	if (SetPixelFormat(hDC, pf, &pfd) == FALSE) {
		MessageBox(NULL, L"SetPixelFormat() failed:  "
			"Cannot set format specified.",
			L"Error", MB_OK);
		return 0;
	}

	DescribePixelFormat(hDC, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	ReleaseDC(hWnd, hDC);

	return hWnd;
}

int main()
{
	HDC hDC; /* device context */
	HGLRC hRC; /* opengl context */
	HWND hWnd; /* window */
	MSG msg; /* message */

	hWnd = CreateOpenGLWindow("minimal", 0, 0, 256, 256, PFD_TYPE_RGBA, 0);
	if (hWnd == NULL)
		exit(1);

	hDC = GetDC(hWnd);
	hRC = wglCreateContext(hDC);
	wglMakeCurrent(hDC, hRC);
	std::cout << load_gl_functions();
	setupQuad();
	loadShader();

	ShowWindow(hWnd, 1);

	while (GetMessage(&msg, hWnd, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	wglMakeCurrent(NULL, NULL);
	ReleaseDC(hWnd, hDC);
	wglDeleteContext(hRC);
	DestroyWindow(hWnd);

	return msg.wParam;
}