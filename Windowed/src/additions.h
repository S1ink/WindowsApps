#pragma once

#include <windowsx.h>
#include <windows.h>
#include <d2d1.h>
#include <math.h>
#pragma comment(lib, "d2d1")

#ifndef UNICODE
#define UNICODE
#endif

template <class T>
void SafeRelease(T** ppT) {
	if (*ppT) {
		(*ppT)->Release();
		*ppT = NULL;
	}
}

namespace graphics {
	struct ShiftingBaseColor {
	public:
		float iterator;
		float color;
		int vector;

		ShiftingBaseColor(float color = 0.5f, int vector = 1, float iterator = 0.01f) : iterator(iterator), color(color), vector(vector) {}

		void iterate(int arg = 1);
	};

	class FullColor {
	public:
		ShiftingBaseColor r, g, b;

		FullColor(float rc, float gc, float bc) : r(ShiftingBaseColor(rc)), g(ShiftingBaseColor(gc)), b(ShiftingBaseColor(bc)) {}

		void iterate(int vector);

		void updateColor(D2D1_COLOR_F& color) {
			color = D2D1::ColorF(r.color, g.color, b.color);
		}

		HRESULT updateBrush(ID2D1HwndRenderTarget** target, ID2D1SolidColorBrush** brush) {
			return (*target)->CreateSolidColorBrush(D2D1::ColorF(r.color, g.color, b.color), brush);
		}
	};

	/*HRESULT brushColor(ID2D1HwndRenderTarget** target, ID2D1SolidColorBrush** brush, D2D1_COLOR_F color) {
		return (*target)->CreateSolidColorBrush(color, brush);
	}

	HRESULT brushColor(ID2D1HwndRenderTarget** target, ID2D1SolidColorBrush** brush, float red, float green, float blue) {
		return (*target)->CreateSolidColorBrush(D2D1::ColorF(red, green, blue), brush);
	}*/
}

namespace wnd {
	template <class DERIVED_TYPE>
	class BaseWindow {
	public:
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
			DERIVED_TYPE* pThis = NULL;

			if (uMsg == WM_NCCREATE) {
				CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
				pThis = (DERIVED_TYPE*)pCreate->lpCreateParams;
				SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);

				pThis->m_hwnd = hwnd;
			}
			else {
				pThis = (DERIVED_TYPE*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
			}
			if (pThis) {
				return pThis->HandleMessage(uMsg, wParam, lParam);
			}
			else {
				return DefWindowProc(hwnd, uMsg, wParam, lParam);
			}
		}

		BaseWindow() : m_hwnd(NULL) { }

		BOOL Create(PCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle = 0, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int nWidth = CW_USEDEFAULT, int nHeight = CW_USEDEFAULT, HWND hWndParent = 0, HMENU hMenu = 0) {
			WNDCLASS wc = { 0 };
			wc.lpfnWndProc = DERIVED_TYPE::WindowProc;
			wc.hInstance = GetModuleHandle(NULL);
			wc.lpszClassName = ClassName();

			RegisterClass(&wc);

			m_hwnd = CreateWindowEx(dwExStyle, ClassName(), lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, GetModuleHandle(NULL), this);

			return (m_hwnd ? TRUE : FALSE);
		}

		HWND Window() const {
			return m_hwnd;
		}

	protected:
		virtual PCWSTR ClassName() const = 0;
		virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

		HWND m_hwnd;
	};

	// * * * * *

	class MainWindow : public BaseWindow<MainWindow> {
		ID2D1Factory* pFactory;
		ID2D1HwndRenderTarget* pRenderTarget;
		ID2D1SolidColorBrush* pBrush;
		
		D2D1_ELLIPSE ellipse;
		D2D1_POINT_2F ptmouse;

		D2D1_COLOR_F e_color, backround;
		graphics::FullColor circle, back;

		HRESULT createGraphics();
		void resetGraphics();

		void LButtonDown(int pixelX, int pixelY, DWORD flags);
		void LButtonUp();
		void mouseMove(int pixelX, int pixelY, DWORD flags);
		void mouseWheel(int vector);
		void paint();
		void resize();

	public:
		MainWindow() : 
			pFactory(NULL), 
			pRenderTarget(NULL), 
			pBrush(NULL), 
			ellipse(D2D1::Ellipse(D2D1::Point2F(), 0, 0)), 
			ptmouse(D2D1::Point2F()), 
			e_color(D2D1::ColorF(0, 0, 0)), 
			backround(e_color),
			circle(graphics::FullColor(0.4f, 0.3f, 0.6f)),
			back(graphics::FullColor(0.7f, 0.2f, 0.5f))
		{}

		PCWSTR ClassName() const { return L"Main Window"; }
		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	};
}

class DPIScale {
	static float scale;

public:
	static void Initialize(HWND* hwnd);

	template<typename T>
	static D2D1_POINT_2F PixelsToDips(T x, T y) {
		return D2D1::Point2F(static_cast<float>(x) / scale, static_cast<float>(y) / scale);
	}
};