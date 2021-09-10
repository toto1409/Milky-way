#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>
#include <math.h>
#pragma warning( disable : 4996 )
#include <strsafe.h>
#pragma warning( default : 4996 )

#define MAX_PLANET 10
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)

LPDIRECT3D9             g_pD3D = NULL;
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL;
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;

struct Planet
{
    LPD3DXMESH sphere;
    LPDIRECT3DTEXTURE9 texture;
    D3DXVECTOR3 position, rotation, scale, rotateValue, NowPosition;
    float revolution, revolutionValue;
};

Planet Planets[10];

struct CUSTOMVERTEX
{
    D3DXVECTOR3 position;
    D3DXVECTOR3 normal;
    float tu;
    float tv;
};



HRESULT CreateMappedSphere(LPDIRECT3DDEVICE9 device, float Radius, UINT slice, UINT stack, LPWSTR str, Planet* planet) //디바이스, 구체의 반지름, z축 기준 나눔, 선 사이 나눔
{
    wchar_t ptr[256];

    wcscpy_s(ptr, L"Textures\\");
    wcscat_s(ptr, str);

    if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, str, &(planet->texture))))
    {
        if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, ptr, &(planet->texture))))
        {
            return E_FAIL;
        }
    }

    LPD3DXMESH temp;

    if (FAILED(D3DXCreateSphere(device, Radius, slice, stack, &temp, NULL)))
    {
        return E_FAIL;
    }

    if (FAILED(temp->CloneMeshFVF(D3DXMESH_SYSTEMMEM, D3DFVF_CUSTOMVERTEX, device, &(planet->sphere))))
    {
        return E_FAIL;
    }

    temp->Release();

    CUSTOMVERTEX* Bvertex;

    if (SUCCEEDED(planet->sphere->LockVertexBuffer(0, (VOID**)&Bvertex)))
    {
        int vertexnum = planet->sphere->GetNumVertices();

        for (int i = 0; i < vertexnum; i++)
        {
            Bvertex->tu = asinf(Bvertex->normal.x) / D3DX_PI + 0.5f;
            Bvertex->tv = asinf(Bvertex->normal.y) / D3DX_PI + 0.5f;

            Bvertex++;
        }

        planet->sphere->UnlockVertexBuffer();
    }

    return S_OK;
}

HRESULT InitD3D( HWND hWnd )
{
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof( d3dpp ) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
    {
        return E_FAIL;
    }

    g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

    return S_OK;
}


void SetPlanet(int num, D3DXVECTOR3 pos, D3DXVECTOR3 rotation, D3DXVECTOR3 scale, D3DXVECTOR3 rotateValue, float revolutionValue)
{
    Planets[num].position.x = pos.x;
    Planets[num].position.y = pos.y;
    Planets[num].position.z = pos.z;
    Planets[num].rotation.x = rotation.x;
    Planets[num].rotation.y = rotation.y;
    Planets[num].rotation.z = rotation.z;
    Planets[num].scale.x = scale.x;
    Planets[num].scale.y = scale.y;
    Planets[num].scale.z = scale.z;
    Planets[num].rotateValue.x = rotateValue.x;
    Planets[num].rotateValue.y = rotateValue.y;
    Planets[num].rotateValue.z = rotateValue.z;
    Planets[num].revolutionValue = revolutionValue;
}

HRESULT InitGeometry()
{
    for (int i = 0; i < MAX_PLANET; i++)
    {
        wchar_t ptr[128];

        wsprintf(ptr, L"%d.bmp", i);


        if (FAILED(CreateMappedSphere(g_pd3dDevice, 2.0f, 30, 30, ptr, &Planets[i])))
        {
            return E_FAIL;
        }
    }

    /*태양*/SetPlanet(0, D3DXVECTOR3(0.0f, 0.0f, 0.0f)/*포지션*/, D3DXVECTOR3(0.0f, 0.0f, 0.0f)/*로테이션*/, D3DXVECTOR3(5.0f, 5.0f, 5.0f)/*스케일*/, D3DXVECTOR3(0.0f, 1.0f, 0.0f)/*자전 값*/, 0.0f/*공전 값*/);
    /*수성*/SetPlanet(1, D3DXVECTOR3(20.0f, 0.0f, -4.0f)/*포지션*/, D3DXVECTOR3(0.0f, 0.0f, 0.0f)/*로테이션*/, D3DXVECTOR3(0.7f, 0.7f, 0.7f)/*스케일*/, D3DXVECTOR3(0.0f, 1.0f, 0.0f)/*자전 값*/, 48.0f/48.0f/*공전 값*/);
    /*금성*/SetPlanet(2, D3DXVECTOR3(25.0f, 0.0f, 7.0f)/*포지션*/, D3DXVECTOR3(0.0f, 0.0f, 0.0f)/*로테이션*/, D3DXVECTOR3(0.9f, 0.9f, 0.9f)/*스케일*/, D3DXVECTOR3(0.0f, 1.0f, 0.0f)/*자전 값*/, 35.0f / 48.0f/*공전 값*/);
    /*지구*/SetPlanet(3, D3DXVECTOR3(33.0f, 0.0f, 0.0f)/*포지션*/, D3DXVECTOR3(0.0f, 0.0f, 0.0f)/*로테이션*/, D3DXVECTOR3(1.0f, 1.0f, 1.0f)/*스케일*/, D3DXVECTOR3(0.0f, 1.0f, 0.0f)/*자전 값*/, 30.0f / 48.0f/*공전 값*/);
    /*화성*/SetPlanet(4, D3DXVECTOR3(37.0f, 0.0f, -10.0f)/*포지션*/, D3DXVECTOR3(0.0f, 0.0f, 0.0f)/*로테이션*/, D3DXVECTOR3(0.8f, 0.8f, 0.8f)/*스케일*/, D3DXVECTOR3(0.0f, 1.0f, 0.0f)/*자전 값*/, 24.0f / 48.0f/*공전 값*/);
    /*목성*/SetPlanet(5, D3DXVECTOR3(47.0f, 0.0f, 4.0f)/*포지션*/, D3DXVECTOR3(0.0f, 0.0f, 0.0f)/*로테이션*/, D3DXVECTOR3(2.5f, 2.5f, 2.5f)/*스케일*/, D3DXVECTOR3(0.0f, 1.0f, 0.0f)/*자전 값*/, 13.0f / 48.0f/*공전 값*/);
    /*토성*/SetPlanet(6, D3DXVECTOR3(60.0f, 0.0f, -13.0f)/*포지션*/, D3DXVECTOR3(0.0f, 0.0f, 0.0f)/*로테이션*/, D3DXVECTOR3(2.2f, 2.2f, 2.2f)/*스케일*/, D3DXVECTOR3(0.0f, 1.0f, 0.0f)/*자전 값*/, 9.7f / 48.0f/*공전 값*/);
    /*천왕성*/SetPlanet(7, D3DXVECTOR3(70.0f, 0.0f, 8.0f)/*포지션*/, D3DXVECTOR3(0.0f, 0.0f, 0.0f)/*로테이션*/, D3DXVECTOR3(1.5f, 1.5f, 1.5f)/*스케일*/, D3DXVECTOR3(0.0f, 1.0f, 0.0f)/*자전 값*/, 6.8f / 48.0f/*공전 값*/);
    /*해왕성*/SetPlanet(8, D3DXVECTOR3(86.0f, 0.0f, 13.0f)/*포지션*/, D3DXVECTOR3(0.0f, 0.0f, 0.0f)/*로테이션*/, D3DXVECTOR3(1.5f, 1.5f, 1.5f)/*스케일*/, D3DXVECTOR3(0.0f, 1.0f, 0.0f)/*자전 값*/, 5.4f / 48.0f/*공전 값*/);
    /*달*/SetPlanet(9, D3DXVECTOR3(3.0f, 0.0f, 3.0f)/*포지션*/, D3DXVECTOR3(0.0f, 0.0f, 0.0f)/*로테이션*/, D3DXVECTOR3(0.2f, 0.2f, 0.2f)/*스케일*/, D3DXVECTOR3(0.0f, 1.0f, 0.0f)/*자전 값*/, 48.0f / 48.0f/*공전 값*/);
    return S_OK;
}



VOID Cleanup()
{
    if( g_pVB != NULL )
        g_pVB->Release();

    if( g_pd3dDevice != NULL )
        g_pd3dDevice->Release();

    if( g_pD3D != NULL )
        g_pD3D->Release();
}


VOID SetupMatrices()
{
    D3DXVECTOR3 vEyePt( 0.0f, 175.0f,-175.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI / 4, 1.0f, 1.0f, 1000.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}


VOID SetupLights()
{
    D3DMATERIAL9 mtrl;
    ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
    mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
    mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
    mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
    mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
    g_pd3dDevice->SetMaterial(&mtrl);

    D3DXVECTOR3 vecDir;
    D3DLIGHT9 light;

    ZeroMemory(&light, sizeof(D3DLIGHT9));
    light.Type = D3DLIGHT_POINT;
        light.Diffuse.r = 1.0f;
        light.Diffuse.g = 1.0f;
        light.Diffuse.b = 1.0f;
        light.Position.x = 0.0f;
        light.Position.y = 0.0f;
        light.Position.z = 0.0f;
        light.Attenuation0 = 1.0f;
    light.Range = 1500.0f;

    g_pd3dDevice->SetLight( 0, &light );
    g_pd3dDevice->LightEnable( 0, TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

    g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00202020 );
}

void RotateAround(Planet* planet, D3DXVECTOR3 axis, D3DXVECTOR3 pos, D3DXMATRIX* matrix)
{
    D3DXMATRIX tr, rt, mat;
    D3DXQUATERNION qr;

    D3DXMatrixTranslation(&tr, -pos.x, -pos.y, -pos.z);
    planet->revolution += planet->revolutionValue;
    D3DXQuaternionRotationAxis(&qr, &axis, D3DXToRadian(planet->revolution));
    D3DXMatrixRotationQuaternion(&rt, &qr);
    D3DXMatrixMultiply(&mat, &tr, &rt);
    D3DXMatrixTranslation(&tr, pos.x, pos.y, pos.z);
    D3DXMatrixMultiply(&mat, &mat, &tr);

    memcpy(matrix, mat, sizeof(D3DXMATRIX));
}

void TransformPlanet(Planet* planet, D3DXMATRIX* matrix)
{
    D3DXMATRIX matWorld, ro, tr;

    D3DXMatrixScaling(&matWorld, planet->scale.x, planet->scale.y, planet->scale.z);

    if (planet->rotateValue.x != 0)
    {
        planet->rotation.x += planet->rotateValue.x;

        D3DXMatrixRotationX(&ro, D3DXToRadian(planet->rotation.x));

        D3DXMatrixMultiply(&matWorld, &matWorld, &ro);
    }



    if (planet->rotateValue.y != 0)
    {
        planet->rotation.y += planet->rotateValue.y;

        D3DXMatrixRotationY(&ro, D3DXToRadian(planet->rotation.y));

        D3DXMatrixMultiply(&matWorld, &matWorld, &ro);
    }



    if (planet->rotateValue.z != 0)
    {
        planet->rotation.z += planet->rotateValue.z;

        D3DXMatrixRotationZ(&ro, D3DXToRadian(planet->rotation.z));

        D3DXMatrixMultiply(&matWorld, &matWorld, &ro);
    }



    D3DXMatrixTranslation(&tr, planet->position.x, planet->position.y, planet->position.z);

    D3DXMatrixMultiply(&matWorld, &matWorld, &tr);

    memcpy(matrix, matWorld, sizeof(D3DXMATRIX));
}


VOID Render()
{
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                         D3DCOLOR_XRGB( 0, 0, 0 ), 1.0f, 0 );

    if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
    {
        SetupLights();

        SetupMatrices();


        for (int i = 0; i < MAX_PLANET; i++)
        {
            D3DXMATRIX mat, ra;
            
            if (i == 9)
            {
                D3DXMATRIX matWorld, ro, tr;
                Planet* planet = &Planets[i];

                D3DXMatrixScaling(&matWorld, planet->scale.x, planet->scale.y, planet->scale.z);

                if (planet->rotateValue.x != 0)
                {
                    planet->rotation.x += planet->rotateValue.x;

                    D3DXMatrixRotationX(&ro, D3DXToRadian(planet->rotation.x));

                    D3DXMatrixMultiply(&matWorld, &matWorld, &ro);
                }



                if (planet->rotateValue.y != 0)
                {
                    planet->rotation.y += planet->rotateValue.y;

                    D3DXMatrixRotationY(&ro, D3DXToRadian(planet->rotation.y));

                    D3DXMatrixMultiply(&matWorld, &matWorld, &ro);
                }



                if (planet->rotateValue.z != 0)
                {
                    planet->rotation.z += planet->rotateValue.z;

                    D3DXMatrixRotationZ(&ro, D3DXToRadian(planet->rotation.z));

                    D3DXMatrixMultiply(&matWorld, &matWorld, &ro);
                }

                D3DXMatrixTranslation(&tr, planet->position.x+Planets[3].NowPosition.x, planet->position.y + Planets[3].NowPosition.y, planet->position.z + Planets[3].NowPosition.z);

                D3DXMatrixMultiply(&matWorld, &matWorld, &tr);

                memcpy(&mat, &matWorld, sizeof(D3DXMATRIX));


                RotateAround(&Planets[i], D3DXVECTOR3(0.0f, 1.0f, 0.0f), Planets[3].NowPosition, &ra);
                D3DXMatrixMultiply(&mat, &mat, &ra);
            }
            else
            {
                TransformPlanet(&Planets[i], &mat);
                RotateAround(&Planets[i], D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), &ra);
                D3DXMatrixMultiply(&mat, &mat, &ra);

            }

            
            D3DXVec3TransformCoord(&Planets[i].NowPosition, new D3DXVECTOR3(0.0f, 0.0f, 0.0f), &mat);


            g_pd3dDevice->SetTransform(D3DTS_WORLD, &mat);
            g_pd3dDevice->SetTexture(0, Planets[i].texture);
            if(i == 0) g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADDSIGNED2X);
            else g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE4X);
            g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
            g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
            g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

            Planets[i].sphere->DrawSubset(0);
        }


        g_pd3dDevice->EndScene();
    }

    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}

LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY:
            Cleanup();
            PostQuitMessage( 0 );
            return 0;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}

INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{
    UNREFERENCED_PARAMETER( hInst );

    WNDCLASSEX wc =
    {
        sizeof( WNDCLASSEX ), CS_CLASSDC, MsgProc, 0L, 0L,
        GetModuleHandle( NULL ), NULL, NULL, NULL, NULL,
        L"MySpace", NULL
    };
    RegisterClassEx( &wc );

    HWND hWnd = CreateWindow( L"MySpace", L"2012302002_강호석",
                              WS_OVERLAPPEDWINDOW, 100, 100, 700, 700,
                              NULL, NULL, wc.hInstance, NULL );

    if( SUCCEEDED( InitD3D( hWnd ) ) )
    {
        if( SUCCEEDED( InitGeometry() ) )
        {
            ShowWindow( hWnd, SW_SHOWDEFAULT );
            UpdateWindow( hWnd );

            MSG msg;
            ZeroMemory( &msg, sizeof( msg ) );
            while( msg.message != WM_QUIT )
            {
                if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
                {
                    TranslateMessage( &msg );
                    DispatchMessage( &msg );
                }
                else
                    Render();
            }
        }
    }

    UnregisterClass( L"D3D Tutorial", wc.hInstance );
    return 0;
}



#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>
#include <math.h>
#pragma warning( disable : 4996 )
#include <strsafe.h>
#pragma warning( default : 4996 )

#define MAX_PLANET 10
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)

LPDIRECT3D9             g_pD3D = NULL;
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL;
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;

struct Planet
{
    LPD3DXMESH sphere;
    LPDIRECT3DTEXTURE9 texture;
    D3DXVECTOR3 position, rotation, scale, rotateValue, NowPosition;
    float revolution, revolutionValue;
};

Planet Planets[10];

struct CUSTOMVERTEX
{
    D3DXVECTOR3 position;
    D3DXVECTOR3 normal;
    float tu;
    float tv;
};



HRESULT CreateMappedSphere(LPDIRECT3DDEVICE9 device, float Radius, UINT slice, UINT stack, LPWSTR str, Planet* planet) //디바이스, 구체의 반지름, z축 기준 나눔, 선 사이 나눔
{
    wchar_t ptr[256];

    wcscpy_s(ptr, L"Textures\\");
    wcscat_s(ptr, str);

    if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, str, &(planet->texture))))
    {
        if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, ptr, &(planet->texture))))
        {
            return E_FAIL;
        }
    }

    LPD3DXMESH temp;

    if (FAILED(D3DXCreateSphere(device, Radius, slice, stack, &temp, NULL)))
    {
        return E_FAIL;
    }

    if (FAILED(temp->CloneMeshFVF(D3DXMESH_SYSTEMMEM, D3DFVF_CUSTOMVERTEX, device, &(planet->sphere))))
    {
        return E_FAIL;
    }

    temp->Release();

    CUSTOMVERTEX* Bvertex;

    if (SUCCEEDED(planet->sphere->LockVertexBuffer(0, (VOID**)&Bvertex)))
    {
        int vertexnum = planet->sphere->GetNumVertices();

        for (int i = 0; i < vertexnum; i++)
        {
            Bvertex->tu = asinf(Bvertex->normal.x) / D3DX_PI + 0.5f;
            Bvertex->tv = asinf(Bvertex->normal.y) / D3DX_PI + 0.5f;

            Bvertex++;
        }

        planet->sphere->UnlockVertexBuffer();
    }

    return S_OK;
}

HRESULT InitD3D( HWND hWnd )
{
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof( d3dpp ) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
    {
        return E_FAIL;
    }

    g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

    return S_OK;
}


void SetPlanet(int num, D3DXVECTOR3 pos, D3DXVECTOR3 rotation, D3DXVECTOR3 scale, D3DXVECTOR3 rotateValue, float revolutionValue)
{
    Planets[num].position.x = pos.x;
    Planets[num].position.y = pos.y;
    Planets[num].position.z = pos.z;
    Planets[num].rotation.x = rotation.x;
    Planets[num].rotation.y = rotation.y;
    Planets[num].rotation.z = rotation.z;
    Planets[num].scale.x = scale.x;
    Planets[num].scale.y = scale.y;
    Planets[num].scale.z = scale.z;
    Planets[num].rotateValue.x = rotateValue.x;
    Planets[num].rotateValue.y = rotateValue.y;
    Planets[num].rotateValue.z = rotateValue.z;
    Planets[num].revolutionValue = revolutionValue;
}

HRESULT InitGeometry()
{
    for (int i = 0; i < MAX_PLANET; i++)
    {
        wchar_t ptr[128];

        wsprintf(ptr, L"%d.bmp", i);


        if (FAILED(CreateMappedSphere(g_pd3dDevice, 2.0f, 30, 30, ptr, &Planets[i])))
        {
            return E_FAIL;
        }
    }

    /*태양*/SetPlanet(0, D3DXVECTOR3(0.0f, 0.0f, 0.0f)/*포지션*/, D3DXVECTOR3(0.0f, 0.0f, 0.0f)/*로테이션*/, D3DXVECTOR3(5.0f, 5.0f, 5.0f)/*스케일*/, D3DXVECTOR3(0.0f, 1.0f, 0.0f)/*자전 값*/, 0.0f/*공전 값*/);
    /*수성*/SetPlanet(1, D3DXVECTOR3(20.0f, 0.0f, -4.0f)/*포지션*/, D3DXVECTOR3(0.0f, 0.0f, 0.0f)/*로테이션*/, D3DXVECTOR3(0.7f, 0.7f, 0.7f)/*스케일*/, D3DXVECTOR3(0.0f, 1.0f, 0.0f)/*자전 값*/, 48.0f/48.0f/*공전 값*/);
    /*금성*/SetPlanet(2, D3DXVECTOR3(25.0f, 0.0f, 7.0f)/*포지션*/, D3DXVECTOR3(0.0f, 0.0f, 0.0f)/*로테이션*/, D3DXVECTOR3(0.9f, 0.9f, 0.9f)/*스케일*/, D3DXVECTOR3(0.0f, 1.0f, 0.0f)/*자전 값*/, 35.0f / 48.0f/*공전 값*/);
    /*지구*/SetPlanet(3, D3DXVECTOR3(33.0f, 0.0f, 0.0f)/*포지션*/, D3DXVECTOR3(0.0f, 0.0f, 0.0f)/*로테이션*/, D3DXVECTOR3(1.0f, 1.0f, 1.0f)/*스케일*/, D3DXVECTOR3(0.0f, 1.0f, 0.0f)/*자전 값*/, 30.0f / 48.0f/*공전 값*/);
    /*화성*/SetPlanet(4, D3DXVECTOR3(37.0f, 0.0f, -10.0f)/*포지션*/, D3DXVECTOR3(0.0f, 0.0f, 0.0f)/*로테이션*/, D3DXVECTOR3(0.8f, 0.8f, 0.8f)/*스케일*/, D3DXVECTOR3(0.0f, 1.0f, 0.0f)/*자전 값*/, 24.0f / 48.0f/*공전 값*/);
    /*목성*/SetPlanet(5, D3DXVECTOR3(47.0f, 0.0f, 4.0f)/*포지션*/, D3DXVECTOR3(0.0f, 0.0f, 0.0f)/*로테이션*/, D3DXVECTOR3(2.5f, 2.5f, 2.5f)/*스케일*/, D3DXVECTOR3(0.0f, 1.0f, 0.0f)/*자전 값*/, 13.0f / 48.0f/*공전 값*/);
    /*토성*/SetPlanet(6, D3DXVECTOR3(60.0f, 0.0f, -13.0f)/*포지션*/, D3DXVECTOR3(0.0f, 0.0f, 0.0f)/*로테이션*/, D3DXVECTOR3(2.2f, 2.2f, 2.2f)/*스케일*/, D3DXVECTOR3(0.0f, 1.0f, 0.0f)/*자전 값*/, 9.7f / 48.0f/*공전 값*/);
    /*천왕성*/SetPlanet(7, D3DXVECTOR3(70.0f, 0.0f, 8.0f)/*포지션*/, D3DXVECTOR3(0.0f, 0.0f, 0.0f)/*로테이션*/, D3DXVECTOR3(1.5f, 1.5f, 1.5f)/*스케일*/, D3DXVECTOR3(0.0f, 1.0f, 0.0f)/*자전 값*/, 6.8f / 48.0f/*공전 값*/);
    /*해왕성*/SetPlanet(8, D3DXVECTOR3(86.0f, 0.0f, 13.0f)/*포지션*/, D3DXVECTOR3(0.0f, 0.0f, 0.0f)/*로테이션*/, D3DXVECTOR3(1.5f, 1.5f, 1.5f)/*스케일*/, D3DXVECTOR3(0.0f, 1.0f, 0.0f)/*자전 값*/, 5.4f / 48.0f/*공전 값*/);
    /*달*/SetPlanet(9, D3DXVECTOR3(3.0f, 0.0f, 3.0f)/*포지션*/, D3DXVECTOR3(0.0f, 0.0f, 0.0f)/*로테이션*/, D3DXVECTOR3(0.2f, 0.2f, 0.2f)/*스케일*/, D3DXVECTOR3(0.0f, 1.0f, 0.0f)/*자전 값*/, 48.0f / 48.0f/*공전 값*/);
    return S_OK;
}



VOID Cleanup()
{
    if( g_pVB != NULL )
        g_pVB->Release();

    if( g_pd3dDevice != NULL )
        g_pd3dDevice->Release();

    if( g_pD3D != NULL )
        g_pD3D->Release();
}


VOID SetupMatrices()
{
    D3DXVECTOR3 vEyePt( 0.0f, 175.0f,-175.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI / 4, 1.0f, 1.0f, 1000.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}


VOID SetupLights()
{
    D3DMATERIAL9 mtrl;
    ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
    mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
    mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
    mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
    mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
    g_pd3dDevice->SetMaterial(&mtrl);

    D3DXVECTOR3 vecDir;
    D3DLIGHT9 light;

    ZeroMemory(&light, sizeof(D3DLIGHT9));
    light.Type = D3DLIGHT_POINT;
        light.Diffuse.r = 1.0f;
        light.Diffuse.g = 1.0f;
        light.Diffuse.b = 1.0f;
        light.Position.x = 0.0f;
        light.Position.y = 0.0f;
        light.Position.z = 0.0f;
        light.Attenuation0 = 1.0f;
    light.Range = 1500.0f;

    g_pd3dDevice->SetLight( 0, &light );
    g_pd3dDevice->LightEnable( 0, TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

    g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00202020 );
}

void RotateAround(Planet* planet, D3DXVECTOR3 axis, D3DXVECTOR3 pos, D3DXMATRIX* matrix)
{
    D3DXMATRIX tr, rt, mat;
    D3DXQUATERNION qr;

    D3DXMatrixTranslation(&tr, -pos.x, -pos.y, -pos.z);
    planet->revolution += planet->revolutionValue;
    D3DXQuaternionRotationAxis(&qr, &axis, D3DXToRadian(planet->revolution));
    D3DXMatrixRotationQuaternion(&rt, &qr);
    D3DXMatrixMultiply(&mat, &tr, &rt);
    D3DXMatrixTranslation(&tr, pos.x, pos.y, pos.z);
    D3DXMatrixMultiply(&mat, &mat, &tr);

    memcpy(matrix, mat, sizeof(D3DXMATRIX));
}

void TransformPlanet(Planet* planet, D3DXMATRIX* matrix)
{
    D3DXMATRIX matWorld, ro, tr;

    D3DXMatrixScaling(&matWorld, planet->scale.x, planet->scale.y, planet->scale.z);

    if (planet->rotateValue.x != 0)
    {
        planet->rotation.x += planet->rotateValue.x;

        D3DXMatrixRotationX(&ro, D3DXToRadian(planet->rotation.x));

        D3DXMatrixMultiply(&matWorld, &matWorld, &ro);
    }



    if (planet->rotateValue.y != 0)
    {
        planet->rotation.y += planet->rotateValue.y;

        D3DXMatrixRotationY(&ro, D3DXToRadian(planet->rotation.y));

        D3DXMatrixMultiply(&matWorld, &matWorld, &ro);
    }



    if (planet->rotateValue.z != 0)
    {
        planet->rotation.z += planet->rotateValue.z;

        D3DXMatrixRotationZ(&ro, D3DXToRadian(planet->rotation.z));

        D3DXMatrixMultiply(&matWorld, &matWorld, &ro);
    }



    D3DXMatrixTranslation(&tr, planet->position.x, planet->position.y, planet->position.z);

    D3DXMatrixMultiply(&matWorld, &matWorld, &tr);

    memcpy(matrix, matWorld, sizeof(D3DXMATRIX));
}


VOID Render()
{
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                         D3DCOLOR_XRGB( 0, 0, 0 ), 1.0f, 0 );

    if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
    {
        SetupLights();

        SetupMatrices();


        for (int i = 0; i < MAX_PLANET; i++)
        {
            D3DXMATRIX mat, ra;
            
            if (i == 9)
            {
                D3DXMATRIX matWorld, ro, tr;
                Planet* planet = &Planets[i];

                D3DXMatrixScaling(&matWorld, planet->scale.x, planet->scale.y, planet->scale.z);

                if (planet->rotateValue.x != 0)
                {
                    planet->rotation.x += planet->rotateValue.x;

                    D3DXMatrixRotationX(&ro, D3DXToRadian(planet->rotation.x));

                    D3DXMatrixMultiply(&matWorld, &matWorld, &ro);
                }



                if (planet->rotateValue.y != 0)
                {
                    planet->rotation.y += planet->rotateValue.y;

                    D3DXMatrixRotationY(&ro, D3DXToRadian(planet->rotation.y));

                    D3DXMatrixMultiply(&matWorld, &matWorld, &ro);
                }



                if (planet->rotateValue.z != 0)
                {
                    planet->rotation.z += planet->rotateValue.z;

                    D3DXMatrixRotationZ(&ro, D3DXToRadian(planet->rotation.z));

                    D3DXMatrixMultiply(&matWorld, &matWorld, &ro);
                }

                D3DXMatrixTranslation(&tr, planet->position.x+Planets[3].NowPosition.x, planet->position.y + Planets[3].NowPosition.y, planet->position.z + Planets[3].NowPosition.z);

                D3DXMatrixMultiply(&matWorld, &matWorld, &tr);

                memcpy(&mat, &matWorld, sizeof(D3DXMATRIX));


                RotateAround(&Planets[i], D3DXVECTOR3(0.0f, 1.0f, 0.0f), Planets[3].NowPosition, &ra);
                D3DXMatrixMultiply(&mat, &mat, &ra);
            }
            else
            {
                TransformPlanet(&Planets[i], &mat);
                RotateAround(&Planets[i], D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), &ra);
                D3DXMatrixMultiply(&mat, &mat, &ra);

            }

            
            D3DXVec3TransformCoord(&Planets[i].NowPosition, new D3DXVECTOR3(0.0f, 0.0f, 0.0f), &mat);


            g_pd3dDevice->SetTransform(D3DTS_WORLD, &mat);
            g_pd3dDevice->SetTexture(0, Planets[i].texture);
            if(i == 0) g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADDSIGNED2X);
            else g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE4X);
            g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
            g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
            g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

            Planets[i].sphere->DrawSubset(0);
        }


        g_pd3dDevice->EndScene();
    }

    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}

LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY:
            Cleanup();
            PostQuitMessage( 0 );
            return 0;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}

INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{
    UNREFERENCED_PARAMETER( hInst );

    WNDCLASSEX wc =
    {
        sizeof( WNDCLASSEX ), CS_CLASSDC, MsgProc, 0L, 0L,
        GetModuleHandle( NULL ), NULL, NULL, NULL, NULL,
        L"MySpace", NULL
    };
    RegisterClassEx( &wc );

    HWND hWnd = CreateWindow( L"MySpace", L"2012302002_강호석",
                              WS_OVERLAPPEDWINDOW, 100, 100, 700, 700,
                              NULL, NULL, wc.hInstance, NULL );

    if( SUCCEEDED( InitD3D( hWnd ) ) )
    {
        if( SUCCEEDED( InitGeometry() ) )
        {
            ShowWindow( hWnd, SW_SHOWDEFAULT );
            UpdateWindow( hWnd );

            MSG msg;
            ZeroMemory( &msg, sizeof( msg ) );
            while( msg.message != WM_QUIT )
            {
                if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
                {
                    TranslateMessage( &msg );
                    DispatchMessage( &msg );
                }
                else
                    Render();
            }
        }
    }

    UnregisterClass( L"D3D Tutorial", wc.hInstance );
    return 0;
}



