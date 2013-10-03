#include "VMRRender.h"

static IVMRWindowlessControl* g_pWc = 0;

VMRRender::VMRRender(HWND xHwnd):hwnd(xHwnd),pGraph(0),pControl(0),pEvent(0)
{
    // Initialize the COM library.
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr))
    {
        printf("ERROR - Could not initialize COM library");
    }

    // Create the filter graph manager and query for interfaces.
    hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, 
        IID_IGraphBuilder, (void **)&pGraph);
    if (FAILED(hr))
    {
        printf("ERROR - Could not create the Filter Graph Manager.");
    }
    
    hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
    hr = pGraph->QueryInterface(IID_IMediaEvent, (void **)&pEvent);
    if (FAILED(hr))
    {
        printf("ERROR - Could not create MediaControl or MediaEvent.");
    }
}

VMRRender::~VMRRender(void)
{
    // Release the VMR interface when you are done.
    g_pWc->Release();
    pControl->Release();
    pEvent->Release();
    pGraph->Release();
    CoUninitialize();
}

HRESULT VMRRender::InitWindowlessVMR( 
                          HWND hwndApp,                  // Window to hold the video. 
                          IGraphBuilder* pxGraph,         // Pointer to the Filter Graph Manager. 
                          IVMRWindowlessControl** ppWc   // Receives a pointer to the VMR.
                          ) 
{ 
    if (!pxGraph || !ppWc) 
    {
        return E_POINTER;
    }
    IBaseFilter* pVmr = NULL; 
    IVMRWindowlessControl* pWc = NULL; 
    // Create the VMR. 
    HRESULT hr = CoCreateInstance(CLSID_VideoMixingRenderer, NULL, 
        CLSCTX_INPROC, IID_IBaseFilter, (void**)&pVmr); 
    if (FAILED(hr))
    {
        return hr;
    }

    // Add the VMR to the filter graph.
    hr = pxGraph->AddFilter(pVmr, L"Video Mixing Renderer"); 
    if (FAILED(hr)) 
    {
        pVmr->Release();
        return hr;
    }
    // Set the rendering mode.  
    IVMRFilterConfig* pConfig; 
    hr = pVmr->QueryInterface(IID_IVMRFilterConfig, (void**)&pConfig); 
    if (SUCCEEDED(hr)) 
    { 
        hr = pConfig->SetRenderingMode(VMRMode_Windowless);
        // force the VMR to go into "mixer mode."
        hr = pConfig->SetNumberOfStreams(1);
        pConfig->Release(); 
    }
    if (SUCCEEDED(hr))
    {
        // Set the window. 
        hr = pVmr->QueryInterface(IID_IVMRWindowlessControl, (void**)&pWc);
        if( SUCCEEDED(hr)) 
        { 
            hr = pWc->SetVideoClippingWindow(hwndApp); 
            if (SUCCEEDED(hr))
            {
                *ppWc = pWc; // Return this as an AddRef'd pointer. 
            }
            else
            {
                // An error occurred, so release the interface.
                pWc->Release();
            }
        } 
    } 
    pVmr->Release(); 
    return hr; 
} 

HRESULT VMRRender::posVideo()
{
    // Find the native video size.
    long lWidth, lHeight; 
    HRESULT hr = g_pWc->GetNativeVideoSize(&lWidth, &lHeight, NULL, NULL); 
    if (SUCCEEDED(hr))
    {
        RECT rcSrc, rcDest; 
        // Set the source rectangle.
        SetRect(&rcSrc, 0, 0, lWidth, lHeight); 

        // Get the window client area.
        GetClientRect(hwnd, &rcDest); 
        // Set the destination rectangle.
        SetRect(&rcDest, 0, 0, rcDest.right, rcDest.bottom); 

        // Set the video position.
        hr = g_pWc->SetVideoPosition(&rcSrc, &rcDest); 
    }
    return hr;
}

HRESULT BlendApplicationImage( 
                              HWND hwndApp,
                              IVMRWindowlessControl* pWc,
                              HBITMAP hbm
                              )
{
    LONG cx, cy;
    HRESULT hr;
    hr = pWc->GetNativeVideoSize(&cx, &cy, NULL, NULL);
    if (FAILED(hr))
        return hr;

    HDC hdc = GetDC(hwndApp);
    if (hdc == NULL)
    {
        return E_FAIL;
    }

    HDC hdcBmp = CreateCompatibleDC(hdc);
    ReleaseDC(hwndApp, hdc);

    if (hdcBmp == NULL)
    {
        return E_FAIL;
    }

    BITMAP bm;
    if (0 == GetObject(hbm, sizeof(bm), &bm))
    {
        DeleteDC(hdcBmp);
        return E_FAIL;
    }

    HBITMAP hbmOld = (HBITMAP)SelectObject(hdcBmp, hbm);
    if (hbmOld == 0)
    {
        DeleteDC(hdcBmp);
        return E_FAIL;
    }

    VMRALPHABITMAP bmpInfo;
    ZeroMemory(&bmpInfo, sizeof(bmpInfo) );
    bmpInfo.dwFlags = VMRBITMAP_HDC;
    bmpInfo.hdc = hdcBmp;

    // Show the entire bitmap in the top-left corner of the video image.
    SetRect(&bmpInfo.rSrc, 0, 0, bm.bmWidth, bm.bmHeight);
    bmpInfo.rDest.left = 0.f;
    bmpInfo.rDest.top = 0.f;
    bmpInfo.rDest.right = (float)bm.bmWidth / (float)cx;
    bmpInfo.rDest.bottom = (float)bm.bmHeight / (float)cy;

    // Set the transparency value (1.0 is opaque, 0.0 is transparent).
    bmpInfo.fAlpha = 0.5f;

    IVMRMixerBitmap* pBmp;
    hr = pWc->QueryInterface(IID_IVMRMixerBitmap, (LPVOID *)&pBmp);
    if (SUCCEEDED(hr)) 
    {
        pBmp->SetAlphaBitmap(&bmpInfo);
        pBmp->Release();
    }

    DeleteObject(SelectObject(hdcBmp, hbmOld));
    DeleteDC(hdcBmp);
    return hr;
}

void VMRRender::buildGraph()
{
    HRESULT hr = InitWindowlessVMR(hwnd, pGraph, &g_pWc);
    if (SUCCEEDED(hr))
    {
        hr = posVideo();
        if (SUCCEEDED(hr))
        {
            // Build the graph. For example:
            pGraph->RenderFile(L"F:\\TDDownload\\Speed.wmv", NULL);

            HBITMAP hbm;
            hbm = (HBITMAP)LoadImage(NULL, "F:\\TDDownload\\test.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            BlendApplicationImage(hwnd, g_pWc, hbm);
            DeleteObject(hbm);

            // Run the graph.
            hr = pControl->Run();
            if (SUCCEEDED(hr))
            {
                // Wait for completion.
                long evCode;
                //pEvent->WaitForCompletion(INFINITE, &evCode);

                // Note: Do not use INFINITE in a real application, because it
                // can block indefinitely.
            }
        }
    }
}

IVMRWindowlessControl* VMRRender::getWC()
{
    return g_pWc;
}
