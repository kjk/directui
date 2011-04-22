#if !defined(AFX_STDAFX_H__E30B2003_188B_4EB4_AB99_3F3734D6CE6C__INCLUDED_)
#define AFX_STDAFX_H__E30B2003_188B_4EB4_AB99_3F3734D6CE6C__INCLUDED_

#define _CRT_SECURE_NO_DEPRECATE

#include "UIlib.h"
#include <olectl.h>
#include <d3d9.h>
#include <math.h>

#define dimof(x) (sizeof(x)/sizeof(*x))
#define MAX max
#define MIN min
#define CLAMP(x,a,b) (MIN(b,MAX(a,x)))

template< class T >
class CSafeRelease
{
public:
   CSafeRelease(T* p) : m_p(p) { };
   ~CSafeRelease() { if (m_p != NULL)  m_p->Release(); };
   T* Detach() { T* t = m_p; m_p = NULL; return t; };
   T* m_p;
};

class AnimationSpooler
{
public:
   AnimationSpooler();
   ~AnimationSpooler();

   enum { MAX_BUFFERS = 40 };

   bool Init(HWND hWnd);
   bool PrepareAnimation(HWND hWnd);
   void CancelJobs();
   bool Render();

   bool IsAnimating() const;
   bool IsJobScheduled() const;
   bool AddJob(AnimJobUI* pJob);

protected:
   void Term();

   COLORREF TranslateColor(LPDIRECT3DSURFACE9 pSurface, COLORREF clr) const;
   bool SetColorKey(LPDIRECT3DTEXTURE9 pTexture, LPDIRECT3DSURFACE9 pSurface, int iTexSize, COLORREF clrColorKey);

   bool PrepareJob_Flat(AnimJobUI* pJob);
   bool RenderJob_Flat(const AnimJobUI* pJob, LPDIRECT3DSURFACE9 pSurface, DWORD dwTick);

protected:
   struct CUSTOMVERTEX 
   {
      FLOAT x, y, z;
      FLOAT rhw;
      DWORD color;
      FLOAT tu, tv;
   };
   typedef CUSTOMVERTEX CUSTOMFAN[4];

   HWND m_hWnd;
   bool m_bIsAnimating;
   bool m_bIsInitialized;
   StdPtrArray m_aJobs;
   D3DFORMAT m_ColorFormat;
   LPDIRECT3D9 m_pD3D;
   LPDIRECT3DDEVICE9 m_p3DDevice;
   LPDIRECT3DSURFACE9 m_p3DBackSurface;
   //
   LPDIRECT3DVERTEXBUFFER9 m_p3DVertices[MAX_BUFFERS];
   LPDIRECT3DTEXTURE9 m_p3DTextures[MAX_BUFFERS];
   CUSTOMFAN m_fans[MAX_BUFFERS];
   int m_nBuffers;
};

#endif // !defined(AFX_STDAFX_H__E30B2003_188B_4EB4_AB99_3F3734D6CE6C__INCLUDED_)
