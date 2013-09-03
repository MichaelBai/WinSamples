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

void VMRRender::buildGraph()
{
    HRESULT hr = InitWindowlessVMR(hwnd, pGraph, &g_pWc);
    if (SUCCEEDED(hr))
    {
        hr = posVideo();
        if (SUCCEEDED(hr))
        {
            // Build the graph. For example:
            pGraph->RenderFile(L"F:\\TDDownload\\test.wmv", NULL);
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
