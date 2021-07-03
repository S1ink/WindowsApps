#include "additions.h"

namespace wnd {
	HRESULT MainWindow::createGraphics() {
		HRESULT hr = S_OK;
		//~if first run or device change
		if (pRenderTarget == NULL) {
			RECT rc;

			GetClientRect(m_hwnd, &rc);
			D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

			hr = pFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(m_hwnd, size), &pRenderTarget);

			if (SUCCEEDED(hr)) {
				hr = pRenderTarget->CreateSolidColorBrush(e_color, &pBrush);
			}
		}
		return hr;
	}

	void MainWindow::resetGraphics() {
		SafeRelease(&pRenderTarget);
		SafeRelease(&pBrush);
	}

	void MainWindow::LButtonDown(int pixelX, int pixelY, DWORD flags) {
		SetCapture(m_hwnd);
		ellipse.point = ptmouse = DPIScale::PixelsToDips(pixelX, pixelY);
		ellipse.radiusX = ellipse.radiusY = 1.0f;
		InvalidateRect(m_hwnd, NULL, FALSE);
	}

	void MainWindow::LButtonUp() {
		ReleaseCapture();
	}

	void MainWindow::mouseMove(int pixelX, int pixelY, DWORD flags) {
		if (flags & MK_LBUTTON) {
			const D2D1_POINT_2F dips = DPIScale::PixelsToDips(pixelX, pixelY);

			const float width = (dips.x - ptmouse.x) / 2;
			const float height = (dips.y - ptmouse.y) / 2;
			const float x1 = ptmouse.x + width;
			const float y1 = ptmouse.y + height;

			ellipse = D2D1::Ellipse(D2D1::Point2F(x1, y1), width, height);

			InvalidateRect(m_hwnd, NULL, FALSE);
		}
	}

	void MainWindow::mouseWheel(int vector) {
		int vec = vector / abs(vector);
		for (int i = 0; i < (abs(vector) / 120); i++) {
			circle.iterate(vec);
			back.iterate(vec);
		}		
		circle.updateBrush(&pRenderTarget, &pBrush);
		back.updateColor(backround);
		paint();
	}

	void MainWindow::paint() {
		HRESULT hr = createGraphics();
		if (SUCCEEDED(hr)) {
			PAINTSTRUCT ps;
			BeginPaint(m_hwnd, &ps);

			pRenderTarget->BeginDraw();

			pRenderTarget->Clear(backround);
			pRenderTarget->FillEllipse(ellipse, pBrush);

			hr = pRenderTarget->EndDraw();
			if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET) {
				resetGraphics();
			}
			EndPaint(m_hwnd, &ps);
		}
	}

	void MainWindow::resize() {
		if (pRenderTarget != NULL) {
			RECT rc;
			GetClientRect(m_hwnd, &rc);

			D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

			pRenderTarget->Resize(size);
			InvalidateRect(m_hwnd, NULL, FALSE);
		}
	}

	LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
		case WM_CREATE:
			DPIScale::Initialize(&m_hwnd);
			if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory))) {
				return -1;
			}
			return 0;
		case WM_LBUTTONDOWN:
			LButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
			return 0;
		case WM_LBUTTONUP:
			LButtonUp();
			return 0;
		case WM_MOUSEMOVE:
			mouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
			return 0;
		case WM_DESTROY:
			resetGraphics();
			SafeRelease(&pFactory);
			PostQuitMessage(0);
			return 0;
		case WM_MOUSEWHEEL:
			mouseWheel(GET_WHEEL_DELTA_WPARAM(wParam));
			return 0;
		case WM_PAINT:
			paint();
			return 0;
		case WM_SIZE:
			resize();
			return 0;
		}
		return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
	}
}

namespace graphics {
	void ShiftingBaseColor::iterate(int arg) {
		color += iterator * vector * arg;
		if (color >= 1.0f || color <= 0) {
			vector *= -1;
		}
	}

	void FullColor::iterate(int vector) {
		r.iterate(vector);
		g.iterate(vector);
		b.iterate(vector);
	}
}

float DPIScale::scale = 1.0f;

void DPIScale::Initialize(HWND* hwnd) {
	scale = GetDpiForWindow(*hwnd) / 96.0f;
}