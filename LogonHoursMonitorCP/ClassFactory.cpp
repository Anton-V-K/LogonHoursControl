#include "stdafx.h"

#include "ClassFactory.h"
#include "Dll.h"
#include "Guids.h"
#include "LogonHoursProvider.h"

ClassFactory::ClassFactory() = default;
ClassFactory::~ClassFactory() = default;

IFACEMETHODIMP ClassFactory::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv)
        return E_INVALIDARG;
    *ppv = nullptr;
    if (riid == IID_IUnknown || riid == IID_IClassFactory)
        *ppv = static_cast<IClassFactory*>(this);
    else
        return E_NOINTERFACE;
    AddRef();
    return S_OK;
}

IFACEMETHODIMP_(ULONG) ClassFactory::AddRef()
{
    return InterlockedIncrement(&m_refCount);
}

IFACEMETHODIMP_(ULONG) ClassFactory::Release()
{
    const long count = InterlockedDecrement(&m_refCount);
    if (count == 0)
        delete this;
    return count;
}

IFACEMETHODIMP ClassFactory::CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppv)
{
    if (!ppv)
        return E_INVALIDARG;
    *ppv = nullptr;
    if (pUnkOuter)
        return CLASS_E_NOAGGREGATION;

    auto* provider = new (std::nothrow) LogonHoursProvider();
    if (!provider)
        return E_OUTOFMEMORY;

    const HRESULT hr = provider->QueryInterface(riid, ppv);
    provider->Release();
    return hr;
}

IFACEMETHODIMP ClassFactory::LockServer(BOOL fLock)
{
    if (fLock)
        DllAddRef();
    else
        DllRelease();
    return S_OK;
}
