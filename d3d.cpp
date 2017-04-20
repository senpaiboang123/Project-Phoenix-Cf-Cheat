#include "d3d.h"

void FillRectangle(int32_t x, int32_t y, int32_t w, int32_t h, D3DCOLOR color, LPDIRECT3DDEVICE9 device)
{
	D3DRECT rect = { x, y, x + w, y + h };
	device->Clear(1, &rect, D3DCLEAR_TARGET, color, 0, 0);
}

void DrawBorder(int32_t x, int32_t y, int32_t w, int32_t h, int32_t p, D3DCOLOR color, LPDIRECT3DDEVICE9 device)
{
	FillRectangle(x, (y + h - p), w, p, color, device);
	FillRectangle(x, y, p, h, color, device);
	FillRectangle(x, y, w, p, color, device);
	FillRectangle(x + w - p, y, p, h, color, device);
}

void DrawBox(int32_t x, int32_t y, int32_t w, int32_t h, D3DCOLOR back_color, D3DCOLOR border_color, LPDIRECT3DDEVICE9 device)
{
	Vertex vertices[4] =
	{
		{ static_cast<float>(x), static_cast<float>(y + h), 0.0f, 0.0f, back_color },
		{ static_cast<float>(x), static_cast<float>(y), 0.0f, 0.0f, back_color },
		{ static_cast<float>(x + w), static_cast<float>(y + h), 0.0f, 0.0f, back_color },
		{ static_cast<float>(x + w), static_cast<float>(y), 0.0f, 0.0f, back_color }
	};

	device->SetRenderState(D3DRS_ALPHABLENDENABLE, D3DZB_TRUE);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	device->SetRenderState(D3DRS_FOGENABLE, D3DZB_FALSE);

	device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	device->SetTexture(0, nullptr);
	device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(Vertex));

	DrawBorder(x, y, w, h, 1, border_color, device);
}

void WriteText(int32_t x, int32_t y, DWORD color, char *text, uint32_t alignment, LPD3DXFONT font)
{
	RECT rect;
	SetRect(&rect, x, y, x, y);

	wchar_t wtext[64];
	mbstowcs_s(nullptr, wtext, text, strlen(text) + 1);

	font->DrawText(0, wtext, -1, &rect, DT_NOCLIP | alignment, color);
}