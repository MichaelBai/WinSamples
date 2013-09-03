#pragma once

#include <DShow.h>

class VMRRender
{
public:
    VMRRender(HWND xHwnd);
    ~VMRRender(void);

    HRESULT InitWindowlessVMR( 
        HWND hwndApp,                  // Window to hold the video. 
        IGraphBuilder* pxGraph,         // Pointer to the Filter Graph Manager. 
        IVMRWindowlessControl** ppWc   // Receives a pointer to the VMR.
    );

    HRESULT posVideo();
    void buildGraph();

    static IVMRWindowlessControl* getWC();
private:
    HWND hwnd;
    IGraphBuilder* pGraph;
    IMediaControl* pControl;
    IMediaEvent* pEvent;
};
