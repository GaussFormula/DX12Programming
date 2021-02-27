#include "d3dApp.h"
#include <DirectXColors.h>

class InitDirect3DApp :public D3DApp
{
public:
    InitDirect3DApp(HINSTANCE hInstance);
    ~InitDirect3DApp();

    virtual bool Initialize()override;

private:
    virtual void OnResize()override;
    virtual void Update(const GameTimer& gt)override;
    virtual void Draw(const GameTimer& gt)override;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR cmdLine, int showCmd)
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG)|defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    try
    {
        InitDirect3DApp theApp(hInstance);
        if (!theApp.Initialize())
        {
            return 0;
        }
        return theApp.Run();
    }
    catch (DxException& e)
    {
        MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
        return 0;
    }
}

InitDirect3DApp::InitDirect3DApp(HINSTANCE hInstance)
    :D3DApp(hInstance)
{

}

InitDirect3DApp::~InitDirect3DApp()
{
    
}

bool InitDirect3DApp::Initialize()
{
    if (!D3DApp::Initialize())
    {
        return false;
    }

    return true;
}

void InitDirect3DApp::OnResize()
{
    D3DApp::OnResize();
}

void InitDirect3DApp::Update(const GameTimer& gt)
{

}

void InitDirect3DApp::Draw(const GameTimer& gt)
{
    // Reuse the memory associated with command recording.
    // We can only reset when the associated command lists have finished execution on the GPU.
    ThrowIfFailed(mDirectCmdListAllocator->Reset());

    // A command list can be reset after it has been added to the command queue via ExecuteCommandList
    ThrowIfFailed(mCommandList->Reset(mDirectCmdListAllocator.Get(), nullptr));

    // Indicate a state transition on the resource usage.
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    mCommandList->ResourceBarrier(1, &barrier);

    // Set the viewport and scissor rect. This needs to be reset whenever the command list is reset.
    mCommandList->RSSetViewports(1, &mScreenViewPort);
    mCommandList->RSSetScissorRects(1, &mScissorRect);

    // Clear the back buffer and depth buffer.
    mCommandList->ClearRenderTargetView(CurrentBackBufferView(), DirectX::Colors::LightSteelBlue, 0, nullptr);
    mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

    // Specify the buffers we are going to render to.
    auto rtvHandle = CurrentBackBufferView();
    auto dsvHandle = DepthStencilView();
    mCommandList->OMSetRenderTargets(1, &rtvHandle, true, &dsvHandle);

    // Indicate a state transition on the resource usage.
    CD3DX12_RESOURCE_BARRIER barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(
        CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    mCommandList->ResourceBarrier(1, &barrier2);

    // Done recording commands.
    ThrowIfFailed(mCommandList->Close());

    // Add the command list to the queue for execution.
    ID3D12CommandList* cmdLists[] = { mCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

    // Swap the back and front buffers.
    ThrowIfFailed(mSwapChain->Present(0, 0));
    mCurrentBackBufferIndex = (mCurrentBackBufferIndex + 1) % SwapChainBufferCount;

    // Wait until frame commands are complete.  This waiting is inefficient and is
    // done for simplicity.  Later we will show how to organize our rendering code
    // so we do not have to wait per frame.
    FlushCommandQueue();
}