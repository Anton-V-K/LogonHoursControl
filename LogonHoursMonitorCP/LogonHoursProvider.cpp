#include "stdafx.h"

#include "Helpers.h"
#include "LogonHoursCredential.h"
#include "LogonHoursProvider.h"

namespace
{
    const CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR s_fields[] =
    {
        { LHCPFI_LABEL, CPFT_LARGE_TEXT, const_cast<LPWSTR>(L"Allowed logon hours") },
        { LHCPFI_HOURS, CPFT_SMALL_TEXT, const_cast<LPWSTR>(L"Hours") },
    };
}

LogonHoursProvider::LogonHoursProvider() = default;

LogonHoursProvider::~LogonHoursProvider()
{
    if (m_credential)
        m_credential->Release();
}

IFACEMETHODIMP LogonHoursProvider::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv)
        return E_INVALIDARG;
    *ppv = nullptr;
    if (riid == IID_IUnknown || riid == IID_ICredentialProvider)
        *ppv = static_cast<ICredentialProvider*>(this);
    else
        return E_NOINTERFACE;
    AddRef();
    return S_OK;
}

IFACEMETHODIMP_(ULONG) LogonHoursProvider::AddRef()
{
    return InterlockedIncrement(&m_refCount);
}

IFACEMETHODIMP_(ULONG) LogonHoursProvider::Release()
{
    const long count = InterlockedDecrement(&m_refCount);
    if (count == 0)
        delete this;
    return count;
}

IFACEMETHODIMP LogonHoursProvider::SetUsageScenario(CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus, DWORD dwFlags)
{
    UNREFERENCED_PARAMETER(dwFlags);

    if (m_credential)
    {
        m_credential->Release();
        m_credential = nullptr;
    }

    switch (cpus)
    {
    case CPUS_LOGON:
    case CPUS_UNLOCK_WORKSTATION:
        m_cpus = cpus;
        return S_OK;
    default:
        m_cpus = CPUS_INVALID;
        return E_NOTIMPL;
    }
}

IFACEMETHODIMP LogonHoursProvider::SetSerialization(const CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION*)
{
    return E_NOTIMPL;
}

IFACEMETHODIMP LogonHoursProvider::Advise(ICredentialProviderEvents*, UINT_PTR)
{
    return S_OK;
}

IFACEMETHODIMP LogonHoursProvider::UnAdvise()
{
    return S_OK;
}

IFACEMETHODIMP LogonHoursProvider::GetFieldDescriptorCount(DWORD* pdwCount)
{
    if (!pdwCount)
        return E_INVALIDARG;
    *pdwCount = LHCPFI_NUM_FIELDS;
    return S_OK;
}

IFACEMETHODIMP LogonHoursProvider::GetFieldDescriptorAt(DWORD dwIndex, CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR** ppcpfd)
{
    if (!ppcpfd)
        return E_INVALIDARG;
    if (dwIndex >= LHCPFI_NUM_FIELDS)
        return E_INVALIDARG;

    return FieldDescriptorCopy(s_fields[dwIndex], ppcpfd);
}

IFACEMETHODIMP LogonHoursProvider::GetCredentialCount(DWORD* pdwCount, DWORD* pdwDefault, BOOL* pbAutoLogonWithDefault)
{
    if (!pdwCount || !pdwDefault || !pbAutoLogonWithDefault)
        return E_INVALIDARG;

    if (m_cpus == CPUS_INVALID)
    {
        *pdwCount = 0;
        *pdwDefault = CREDENTIAL_PROVIDER_NO_DEFAULT;
        *pbAutoLogonWithDefault = FALSE;
        return S_OK;
    }

    *pdwCount = 1;
    *pdwDefault = CREDENTIAL_PROVIDER_NO_DEFAULT;
    *pbAutoLogonWithDefault = FALSE;
    return S_OK;
}

IFACEMETHODIMP LogonHoursProvider::GetCredentialAt(DWORD dwIndex, ICredentialProviderCredential** ppcpc)
{
    if (!ppcpc || dwIndex != 0 || m_cpus == CPUS_INVALID)
        return E_INVALIDARG;

    if (!m_credential)
    {
        auto* credential = new (std::nothrow) LogonHoursCredential();
        if (!credential)
            return E_OUTOFMEMORY;
        m_credential = credential;
    }

    return m_credential->QueryInterface(IID_ICredentialProviderCredential, reinterpret_cast<void**>(ppcpc));
}
