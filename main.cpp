// example how to set up D3D11 rendering on Windows in C
#include "geometry.h"
#include "input.h"

#define COBJMACROS
#include <d3d11.h>
#include <dxgi1_3.h>
#include <d3dcompiler.h>
#include <dxgidebug.h>
#include <directxmath.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <float.h>
#include <string.h>
#include <stddef.h>

// replace this with your favorite Assert() implementation
#include <intrin.h>
#include <vector>
#include <iostream>
#define Assert(cond) do { if (!(cond)) __debugbreak(); } while (0)
#define AssertHR(hr) Assert(SUCCEEDED(hr))

#pragma comment (lib, "gdi32")
#pragma comment (lib, "user32")
#pragma comment (lib, "dxguid")
#pragma comment (lib, "dxgi")
#pragma comment (lib, "d3d11")
#pragma comment (lib, "d3dcompiler")

#define STR2(x) #x
#define STR(x) STR2(x)

static void FatalError(const char *message) {
  MessageBoxA(NULL, message, "Error", MB_ICONEXCLAMATION);
  ExitProcess(0);
}

static LRESULT CALLBACK WindowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam) {
  switch (msg) {
    case WM_DESTROY:PostQuitMessage(0);
      return 0;
    default:
      input::OnInput(msg, wparam, lparam);
  }

  return DefWindowProcW(wnd, msg, wparam, lparam);
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previnstance, LPSTR cmdline, int cmdshow) {
  // Allocate a console for this application
  AllocConsole();

  // Redirect the CRT standard input, output and error handles to the console
  freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
  freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
  freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);

  // register window class to have custom WindowProc callback
  WNDCLASSEXW wc =
      {
          .cbSize = sizeof(wc),
          .lpfnWndProc = WindowProc,
          .hInstance = instance,
          .hIcon = LoadIcon(NULL, IDI_APPLICATION),
          .hCursor = LoadCursor(NULL, IDC_ARROW),
          .lpszClassName = L"d3d11_window_class",
      };
  ATOM atom = RegisterClassExW(&wc);
  Assert(atom && "Failed to register window class");

  // window properties - width, height and style
  int width = CW_USEDEFAULT;
  int height = CW_USEDEFAULT;
  // WS_EX_NOREDIRECTIONBITMAP flag here is needed to fix ugly bug with Windows 10
  // when window is resized and DXGI swap chain uses FLIP presentation model
  // DO NOT use it if you choose to use non-FLIP presentation model
  // read about the bug here: https://stackoverflow.com/q/63096226 and here: https://stackoverflow.com/q/53000291
  DWORD exstyle = WS_EX_APPWINDOW | WS_EX_NOREDIRECTIONBITMAP;
  DWORD style = WS_OVERLAPPEDWINDOW;

  // uncomment in case you want fixed size window
  //style &= ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;
  //RECT rect = { 0, 0, 1280, 720 };
  //AdjustWindowRectEx(&rect, style, FALSE, exstyle);
  //width = rect.right - rect.left;
  //height = rect.bottom - rect.top;

  // create window
  HWND window = CreateWindowExW(
      exstyle, wc.lpszClassName, L"D3D11 Window", style,
      CW_USEDEFAULT, CW_USEDEFAULT, width, height,
      NULL, NULL, wc.hInstance, NULL);
  Assert(window && "Failed to create window");

  HRESULT hr;

  ID3D11Device *device;
  ID3D11DeviceContext *context;

  // create D3D11 device & context
  {
    UINT flags = 0;
#ifndef NDEBUG
    // this enables VERY USEFUL debug messages in debugger output
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    D3D_FEATURE_LEVEL levels[] = {D3D_FEATURE_LEVEL_11_0};
    hr = D3D11CreateDevice(
        NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, levels, ARRAYSIZE(levels),
        D3D11_SDK_VERSION, &device, NULL, &context);
    // make sure device creation succeeeds before continuing
    // for simple applciation you could retry device creation with
    // D3D_DRIVER_TYPE_WARP driver type which enables software rendering
    // (could be useful on broken drivers or remote desktop situations)
    AssertHR(hr);
  }

#ifndef NDEBUG
  // for debug builds enable VERY USEFUL debug break on API errors
  {
    ID3D11InfoQueue *info;
    device->QueryInterface(IID_PPV_ARGS(&info));
    info->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
    info->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, TRUE);
    info->Release();
  }

  // enable debug break for DXGI too
  {
    IDXGIInfoQueue *dxgiInfo;
    hr = DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiInfo));
    AssertHR(hr);
    dxgiInfo->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, TRUE);
    dxgiInfo->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, TRUE);
    dxgiInfo->Release();
  }

  // after this there's no need to check for any errors on device functions manually
  // so all HRESULT return  values in this code will be ignored
  // debugger will break on errors anyway
#endif

  // create DXGI swap chain
  IDXGISwapChain1 *swapChain;
  {
    // get DXGI device from D3D11 device
    IDXGIDevice *dxgiDevice;
    hr = device->QueryInterface(IID_PPV_ARGS(&dxgiDevice));
    AssertHR(hr);

    // get DXGI adapter from DXGI device
    IDXGIAdapter *dxgiAdapter;
    hr = dxgiDevice->GetAdapter(&dxgiAdapter);
    AssertHR(hr);

    // get DXGI factory from DXGI adapter
    IDXGIFactory2 *factory;
    hr = dxgiAdapter->GetParent(IID_PPV_ARGS(&factory));
    AssertHR(hr);

    DXGI_SWAP_CHAIN_DESC1 desc =
        {
            // default 0 value for width & height means to get it from HWND automatically
            //.Width = 0,
            //.Height = 0,

            // or use DXGI_FORMAT_R8G8B8A8_UNORM_SRGB for storing sRGB
            .Format = DXGI_FORMAT_R8G8B8A8_UNORM,

            // FLIP presentation model does not allow MSAA framebuffer
            // if you want MSAA then you'll need to render offscreen and manually
            // resolve to non-MSAA framebuffer
            .SampleDesc = {1, 0},

            .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
            .BufferCount = 2,

            // we don't want any automatic scaling of window content
            // this is supported only on FLIP presentation model
            .Scaling = DXGI_SCALING_NONE,

            // use more efficient FLIP presentation model
            // Windows 10 allows to use DXGI_SWAP_EFFECT_FLIP_DISCARD
            // for Windows 8 compatibility use DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL
            // for Windows 7 compatibility use DXGI_SWAP_EFFECT_DISCARD
            .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
        };

    hr = factory->CreateSwapChainForHwnd((IUnknown *) device, window, &desc, NULL, NULL, &swapChain);
    AssertHR(hr);

    // disable silly Alt+Enter changing monitor resolution to match window size
    factory->MakeWindowAssociation(window, DXGI_MWA_NO_ALT_ENTER);

    factory->Release();
    dxgiAdapter->Release();
    dxgiDevice->Release();
  }

  GeometryBuilder geometryBuilder = {};

  geometryBuilder.AddQuad({
                              {0.5f, 0.5f},
                              {0.5f, -0.5f},
                              {-0.5f, -0.5f},
                              {-0.5f, 0.5f}
                          });

  ID3D11Buffer *vertex_buffer;
  {
    D3D11_BUFFER_DESC desc =
        {
            .ByteWidth = geometryBuilder.GetVerticesSize(),
            .Usage = D3D11_USAGE_IMMUTABLE,
            .BindFlags = D3D11_BIND_VERTEX_BUFFER,
        };

    D3D11_SUBRESOURCE_DATA initial = {.pSysMem = geometryBuilder.vertices.data()};
    device->CreateBuffer(&desc, &initial, &vertex_buffer);
  }

  ID3D11Buffer *indices_buffer;
  {
    D3D11_BUFFER_DESC desc =
        {
            .ByteWidth = static_cast<UINT>(geometryBuilder.indices.size() * sizeof(uint32_t)),
            .Usage = D3D11_USAGE_IMMUTABLE,
            .BindFlags = D3D11_BIND_INDEX_BUFFER,
        };

    D3D11_SUBRESOURCE_DATA initial = {.pSysMem = geometryBuilder.indices.data()};
    device->CreateBuffer(&desc, &initial, &indices_buffer);
  }

  // vertex & pixel shaders for drawing triangle, plus input layout for vertex input
  ID3D11InputLayout *layout;
  ID3D11VertexShader *vshader;
  ID3D11PixelShader *pshader;
  {
    // these must match vertex shader input layout (VS_INPUT in vertex shader source below)
    D3D11_INPUT_ELEMENT_DESC desc[] =
        {
            {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, static_cast<UINT>(offsetof(struct Vertex, position)), D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, static_cast<UINT>(offsetof(struct Vertex, uv)), D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, static_cast<UINT>(offsetof(struct Vertex, color)), D3D11_INPUT_PER_VERTEX_DATA, 0},
        };

#if 0
    // alternative to hlsl compilation at runtime is to precompile shaders offline
        // it improves startup time - no need to parse hlsl files at runtime!
        // and it allows to remove runtime dependency on d3dcompiler dll file

        // a) save shader source code into "shader.hlsl" file
        // b) run hlsl compiler to compile shader, these run compilation with optimizations and without debug info:
        //      fxc.exe /nologo /T vs_5_0 /E vs /O3 /WX /Zpc /Ges /Fh d3d11_vshader.h /Vn d3d11_vshader /Qstrip_reflect /Qstrip_debug /Qstrip_priv shader.hlsl
        //      fxc.exe /nologo /T ps_5_0 /E ps /O3 /WX /Zpc /Ges /Fh d3d11_pshader.h /Vn d3d11_pshader /Qstrip_reflect /Qstrip_debug /Qstrip_priv shader.hlsl
        //    they will save output to d3d11_vshader.h and d3d11_pshader.h files
        // c) change #if 0 above to #if 1

        // you can also use "/Fo d3d11_*shader.bin" argument to save compiled shader as binary file to store with your assets
        // then provide binary data for Create*Shader functions below without need to include shader bytes in C

#include "d3d11_vshader.h"
#include "d3d11_pshader.h"

        ID3D11Device_CreateVertexShader(device, d3d11_vshader, sizeof(d3d11_vshader), NULL, &vshader);
        ID3D11Device_CreatePixelShader(device, d3d11_pshader, sizeof(d3d11_pshader), NULL, &pshader);
        ID3D11Device_CreateInputLayout(device, desc, ARRAYSIZE(desc), d3d11_vshader, sizeof(d3d11_vshader), &layout);
#else
    const char hlsl[] =
        "#line " STR(__LINE__) "                                  \n\n" // actual line number in this file for nicer error messages
        "                                                           \n"
        "struct VS_INPUT                                            \n"
        "{                                                          \n"
        "     float2 pos   : POSITION;                              \n" // these names must match D3D11_INPUT_ELEMENT_DESC array
        "     float2 uv    : TEXCOORD;                              \n"
        "     float3 color : COLOR;                                 \n"
        "};                                                         \n"
        "                                                           \n"
        "struct PS_INPUT                                            \n"
        "{                                                          \n"
        "  float4 pos   : SV_POSITION;                              \n" // these names do not matter, except SV_... ones
        "  float2 uv    : TEXCOORD;                                 \n"
        "  float4 color : COLOR;                                    \n"
        "};                                                         \n"
        "                                                           \n"
        "cbuffer cbuffer0 : register(b0)                            \n" // b0 = constant buffer bound to slot 0
        "{                                                          \n"
        "    float4x4 uTransform;                                   \n"
        "}                                                          \n"
        "                                                           \n"
        "sampler sampler0 : register(s0);                           \n" // s0 = sampler bound to slot 0
        "                                                           \n"
        "Texture2D<float4> texture0 : register(t0);                 \n" // t0 = shader resource bound to slot 0
        "                                                           \n"
        "PS_INPUT vs(VS_INPUT input)                                \n"
        "{                                                          \n"
        "    PS_INPUT output;                                       \n"
        "    output.pos = mul(uTransform, float4(input.pos, 0, 1)); \n"
        "    output.uv = input.uv;                                  \n"
        "    output.color = float4(input.color, 1);                 \n"
        "    return output;                                         \n"
        "}                                                          \n"
        "                                                           \n"
        "float4 ps(PS_INPUT input) : SV_TARGET                      \n"
        "{                                                          \n"
        "    float4 tex = texture0.Sample(sampler0, input.uv);      \n"
        "    return input.color * tex;                              \n"
        "}                                                          \n";;

    UINT flags = D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR | D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_WARNINGS_ARE_ERRORS;
#ifndef NDEBUG
    flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

    ID3DBlob *error;

    ID3DBlob *vblob;
    hr = D3DCompile(hlsl, sizeof(hlsl), NULL, NULL, NULL, "vs", "vs_5_0", flags, 0, &vblob, &error);
    if (FAILED(hr)) {
      const char *message = (const char *) error->GetBufferPointer(); // ID3D10Blob_GetBufferPointer(error);
      OutputDebugStringA(message);
      Assert(!"Failed to compile vertex shader!");
    }

    ID3DBlob *pblob;
    hr = D3DCompile(hlsl, sizeof(hlsl), NULL, NULL, NULL, "ps", "ps_5_0", flags, 0, &pblob, &error);
    if (FAILED(hr)) {
      const char *message = (const char *) error->GetBufferPointer();
      OutputDebugStringA(message);
      Assert(!"Failed to compile pixel shader!");
    }

    device->CreateVertexShader(vblob->GetBufferPointer(), vblob->GetBufferSize(), NULL, &vshader);
    device->CreatePixelShader(pblob->GetBufferPointer(), pblob->GetBufferSize(), NULL, &pshader);
    device->CreateInputLayout(desc, ARRAYSIZE(desc), vblob->GetBufferPointer(), vblob->GetBufferSize(), &layout);

    pblob->Release();
    vblob->Release();
#endif
  }

  ID3D11Buffer *ubuffer;
  {
    D3D11_BUFFER_DESC desc =
        {
            // space for 4x4 float matrix (cbuffer0 from pixel shader)
            .ByteWidth = 4 * 4 * sizeof(float),
            .Usage = D3D11_USAGE_DYNAMIC,
            .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
            .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
        };
    device->CreateBuffer(&desc, NULL, &ubuffer);
  }

  ID3D11ShaderResourceView *textureView;
  {
    // checkerboard texture, with 50% transparency on black colors
    unsigned int pixels[] =
        {
            0x80000000, 0xffffffff,
            0xffffffff, 0x80000000,
        };
    UINT width = 2;
    UINT height = 2;

    D3D11_TEXTURE2D_DESC desc =
        {
            .Width = width,
            .Height = height,
            .MipLevels = 1,
            .ArraySize = 1,
            .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
            .SampleDesc = {1, 0},
            .Usage = D3D11_USAGE_IMMUTABLE,
            .BindFlags = D3D11_BIND_SHADER_RESOURCE,
        };

    D3D11_SUBRESOURCE_DATA data =
        {
            .pSysMem = pixels,
            .SysMemPitch = static_cast<UINT>(width * sizeof(unsigned int)),
        };

    ID3D11Texture2D *texture;
    device->CreateTexture2D(&desc, &data, &texture);
    device->CreateShaderResourceView((ID3D11Resource *) texture, NULL, &textureView);
    texture->Release();
  }

  ID3D11SamplerState *sampler;
  {
    D3D11_SAMPLER_DESC desc =
        {
            .Filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
            .AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
            .AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
            .AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
            .MaxAnisotropy = 1,
            .MinLOD = -FLT_MAX,
            .MaxLOD = +FLT_MAX,
        };

    device->CreateSamplerState(&desc, &sampler);
  }

  ID3D11BlendState *blendState;
  {
    // enable alpha blending
    D3D11_BLEND_DESC desc = {};
    desc.RenderTarget[0] = {
        .BlendEnable = TRUE,
        .SrcBlend = D3D11_BLEND_SRC_ALPHA,
        .DestBlend = D3D11_BLEND_INV_SRC_ALPHA,
        .BlendOp = D3D11_BLEND_OP_ADD,
        .SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA,
        .DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA,
        .BlendOpAlpha = D3D11_BLEND_OP_ADD,
        .RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL,
    };
    device->CreateBlendState(&desc, &blendState);
  }

  ID3D11RasterizerState *rasterizerState;
  {
    // disable culling
    D3D11_RASTERIZER_DESC desc =
        {
            .FillMode = D3D11_FILL_SOLID,
            .CullMode = D3D11_CULL_NONE,
        };
    device->CreateRasterizerState(&desc, &rasterizerState);
  }

  ID3D11DepthStencilState *depthState;
  {
    // disable depth & stencil test
    D3D11_DEPTH_STENCIL_DESC desc =
        {
            .DepthEnable = FALSE,
            .DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL,
            .DepthFunc = D3D11_COMPARISON_LESS,
            .StencilEnable = FALSE,
            .StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK,
            .StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK,
            // .FrontFace = ...
            // .BackFace = ...
        };
    device->CreateDepthStencilState(&desc, &depthState);
  }

  ID3D11RenderTargetView *rtView = NULL;
  ID3D11DepthStencilView *dsView = NULL;

  // show the window
  ShowWindow(window, SW_SHOWDEFAULT);

  LARGE_INTEGER freq, c1;
  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&c1);

  float angle = 0;
  DWORD currentWidth = 0;
  DWORD currentHeight = 0;

  for (;;) {
    input::Update();

    // process all incoming Windows messages
    MSG msg;
    if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
      if (msg.message == WM_QUIT) {
        break;
      }
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
    }

    // get current size for window client area
    RECT rect;
    GetClientRect(window, &rect);
    width = rect.right - rect.left;
    height = rect.bottom - rect.top;

    // resize swap chain if needed
    if (rtView == NULL || width != currentWidth || height != currentHeight) {
      if (rtView) {
        // release old swap chain buffers
        context->ClearState();
        rtView->Release();
        dsView->Release();
        rtView = NULL;
      }

      // resize to new size for non-zero size
      if (width != 0 && height != 0) {
        hr = swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
        if (FAILED(hr)) {
          FatalError("Failed to resize swap chain!");
        }

        // create RenderTarget view for new backbuffer texture
        ID3D11Texture2D *backbuffer;
        swapChain->GetBuffer(0, IID_PPV_ARGS(&backbuffer));
        device->CreateRenderTargetView((ID3D11Resource *) backbuffer, NULL, &rtView);
        backbuffer->Release();

        D3D11_TEXTURE2D_DESC depthDesc =
            {
                .Width = (UINT) width,
                .Height = (UINT) height,
                .MipLevels = 1,
                .ArraySize = 1,
                .Format = DXGI_FORMAT_D32_FLOAT, // or use DXGI_FORMAT_D32_FLOAT_S8X24_UINT if you need stencil
                .SampleDesc = {1, 0},
                .Usage = D3D11_USAGE_DEFAULT,
                .BindFlags = D3D11_BIND_DEPTH_STENCIL,
            };

        // create new depth stencil texture & DepthStencil view
        ID3D11Texture2D *depth;
        device->CreateTexture2D(&depthDesc, NULL, &depth);
        device->CreateDepthStencilView((ID3D11Resource *) depth, NULL, &dsView);
        depth->Release();
      }

      currentWidth = width;
      currentHeight = height;
    }

    // can render only if window size is non-zero - we must have backbuffer & RenderTarget view created
    if (rtView) {
      LARGE_INTEGER c2;
      QueryPerformanceCounter(&c2);
      float delta = (float) ((double) (c2.QuadPart - c1.QuadPart) / freq.QuadPart);
      c1 = c2;

      // output viewport covering all client area of window
      D3D11_VIEWPORT viewport =
          {
              .TopLeftX = 0,
              .TopLeftY = 0,
              .Width = (FLOAT) width,
              .Height = (FLOAT) height,
              .MinDepth = 0,
              .MaxDepth = 1,
          };

      // clear screen
      FLOAT color[] = {0.392f, 0.584f, 0.929f, 1.f};
      context->ClearRenderTargetView(rtView, color);
      context->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

      // setup 4x4c rotation matrix in uniform
      {
        if (input::IsMouseButtonHeld(input::MouseButton::RIGHT)) {
          angle += input::GetMouseDelta().X * 0.01f;
        }
        angle = fmodf(angle, 2.0f * (float) Math::Pi);

        float aspect = (float) height / width;
        // View matrix.
        static float time = 0.0f;
        time += 0.016f;
        float camX = input::GetMousePosition().X * 0.01f;
        float camZ = input::GetMousePosition().Y * 0.01f;

        DirectX::XMMATRIX model = DirectX::XMMatrixRotationY(angle);

        DirectX::XMFLOAT3 cam_pos(camX, 0.f, camZ);  // Update camera position
        DirectX::XMVECTOR v_cam_pos = DirectX::XMLoadFloat3(&cam_pos);

        DirectX::XMFLOAT3 cam_target(0.f, 0.f, 0.f);
        DirectX::XMVECTOR v_cam_target = DirectX::XMLoadFloat3(&cam_target);

        DirectX::XMFLOAT3 cam_up(0.f, 1.f, 0.f);
        DirectX::XMVECTOR v_cam_up = DirectX::XMLoadFloat3(&cam_up);

        DirectX::XMMATRIX view =DirectX::XMMatrixLookAtRH(v_cam_pos, v_cam_target, v_cam_up);

        // Projection matrix
        DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveFovRH(DirectX::XMConvertToRadians(45.f), aspect, 0.1f, 100.f);

        auto final_matrix = model * view * projection;

        D3D11_MAPPED_SUBRESOURCE mapped;
        context->Map((ID3D11Resource *) ubuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
        memcpy(mapped.pData, &final_matrix, sizeof(final_matrix));
        context->Unmap((ID3D11Resource *) ubuffer, 0);
      }

      // Input Assembler
      context->IASetInputLayout(layout);
      context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
      UINT stride = sizeof(struct Vertex);
      UINT offset = 0;
      context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
      context->IASetIndexBuffer(indices_buffer, DXGI_FORMAT_R32_UINT, 0);

      // Vertex Shader
      context->VSSetConstantBuffers(0, 1, &ubuffer);
      context->VSSetShader(vshader, NULL, 0);

      // Rasterizer Stage
      context->RSSetViewports(1, &viewport);
      context->RSSetState(rasterizerState);

      // Pixel Shader
      context->PSSetSamplers(0, 1, &sampler);
      context->PSSetShaderResources(0, 1, &textureView);
      context->PSSetShader(pshader, NULL, 0);

      // Output Merger
      context->OMSetBlendState(blendState, NULL, ~0U);
      context->OMSetDepthStencilState(depthState, 0);
      context->OMSetRenderTargets(1, &rtView, dsView);

      // draw 3 vertices
      context->DrawIndexed(geometryBuilder.indices.size(), 0, 0);
    }

    // change to FALSE to disable vsync
    BOOL vsync = TRUE;
    hr = swapChain->Present(vsync ? 1 : 0, 0);
    if (hr == DXGI_STATUS_OCCLUDED) {
      // window is minimized, cannot vsync - instead sleep a bit
      if (vsync) {
        Sleep(10);
      }
    }
    else if (FAILED(hr)) {
      FatalError("Failed to present swap chain! Device lost?");
    }
  }
}
