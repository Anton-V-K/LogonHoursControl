#include "stdafx.h"

#include <Common/LogonHoursText.h>
#include <Common/UserAccounts.h>

#include "LogonHoursCredential.h"

LogonHoursCredential::LogonHoursCredential() = default;
LogonHoursCredential::~LogonHoursCredential()
{
    if (m_events)
        m_events->Release();
}

void LogonHoursCredential::EnsureHoursLoaded()
{
    if (m_hoursLoaded)
        return;
    m_hoursLoaded = true;

    m_label = L"Allowed logon hours";

    std::map<std::wstring, LogonHours> hours;
    if (!LoadLogonHoursForUsers(hours))
        m_hoursText = L"(unable to read logon hours)";
    else
        m_hoursText = FormatAllUsersLogonHours(hours);
}

HRESULT LogonHoursCredential::DuplicateString(const wchar_t* source, wchar_t** target)
{
    if (!source || !target)
        return E_INVALIDARG;
    const size_t len = wcslen(source) + 1;
    wchar_t* copy = static_cast<wchar_t*>(CoTaskMemAlloc(len * sizeof(wchar_t)));
    if (!copy)
        return E_OUTOFMEMORY;
    wcscpy_s(copy, len, source);
    *target = copy;
    return S_OK;
}

IFACEMETHODIMP LogonHoursCredential::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv)
        return E_INVALIDARG;
    *ppv = nullptr;
    if (riid == IID_IUnknown || riid == IID_ICredentialProviderCredential)
        *ppv = static_cast<ICredentialProviderCredential*>(this);
    else
        return E_NOINTERFACE;
    AddRef();
    return S_OK;
}

IFACEMETHODIMP_(ULONG) LogonHoursCredential::AddRef()
{
    return InterlockedIncrement(&m_refCount);
}

IFACEMETHODIMP_(ULONG) LogonHoursCredential::Release()
{
    const long count = InterlockedDecrement(&m_refCount);
    if (count == 0)
        delete this;
    return count;
}

IFACEMETHODIMP LogonHoursCredential::Advise(ICredentialProviderCredentialEvents* pcpce)
{
    if (m_events)
        m_events->Release();
    m_events = pcpce;
    if (m_events)
        m_events->AddRef();
    return S_OK;
}

IFACEMETHODIMP LogonHoursCredential::UnAdvise()
{
    if (m_events)
    {
        m_events->Release();
        m_events = nullptr;
    }
    return S_OK;
}

IFACEMETHODIMP LogonHoursCredential::SetSelected(BOOL* pbAutoLogon)
{
    if (pbAutoLogon)
        *pbAutoLogon = FALSE;
    return S_OK;
}

IFACEMETHODIMP LogonHoursCredential::SetDeselected()
{
    return S_OK;
}

IFACEMETHODIMP LogonHoursCredential::GetFieldState(DWORD dwFieldID, CREDENTIAL_PROVIDER_FIELD_STATE* pcpfs,
    CREDENTIAL_PROVIDER_FIELD_INTERACTIVE_STATE* pcpfis)
{
    if (!pcpfs || !pcpfis)
        return E_INVALIDARG;

    switch (dwFieldID)
    {
    case LHCPFI_LABEL:
        *pcpfs = CPFS_DISPLAY_IN_BOTH;
        *pcpfis = CPFIS_READONLY;
        return S_OK;
    case LHCPFI_HOURS:
        *pcpfs = CPFS_DISPLAY_IN_BOTH;
        *pcpfis = CPFIS_READONLY;
        return S_OK;
    default:
        return E_INVALIDARG;
    }
}

IFACEMETHODIMP LogonHoursCredential::GetStringValue(DWORD dwFieldID, LPWSTR* ppwsz)
{
    if (!ppwsz)
        return E_INVALIDARG;

    EnsureHoursLoaded();

    switch (dwFieldID)
    {
    case LHCPFI_LABEL:
        return DuplicateString(m_label.c_str(), ppwsz);
    case LHCPFI_HOURS:
        return DuplicateString(m_hoursText.c_str(), ppwsz);
    default:
        return E_INVALIDARG;
    }
}

IFACEMETHODIMP LogonHoursCredential::GetBitmapValue(DWORD, HBITMAP* phbmp)
{
    if (phbmp)
        *phbmp = nullptr;
    return S_OK;
}

IFACEMETHODIMP LogonHoursCredential::GetCheckboxValue(DWORD, BOOL*, LPWSTR*)
{
    return E_NOTIMPL;
}

IFACEMETHODIMP LogonHoursCredential::GetComboBoxValueCount(DWORD, DWORD*, DWORD*)
{
    return E_NOTIMPL;
}

IFACEMETHODIMP LogonHoursCredential::GetComboBoxValueAt(DWORD, DWORD, LPWSTR*)
{
    return E_NOTIMPL;
}

IFACEMETHODIMP LogonHoursCredential::GetSubmitButtonValue(DWORD, DWORD*)
{
    return E_NOTIMPL;
}

IFACEMETHODIMP LogonHoursCredential::SetStringValue(DWORD, LPCWSTR)
{
    return E_NOTIMPL;
}

IFACEMETHODIMP LogonHoursCredential::SetCheckboxValue(DWORD, BOOL)
{
    return E_NOTIMPL;
}

IFACEMETHODIMP LogonHoursCredential::SetComboBoxSelectedValue(DWORD, DWORD)
{
    return E_NOTIMPL;
}

IFACEMETHODIMP LogonHoursCredential::CommandLinkClicked(DWORD)
{
    return E_NOTIMPL;
}

IFACEMETHODIMP LogonHoursCredential::GetSerialization(CREDENTIAL_PROVIDER_GET_SERIALIZATION_RESPONSE* pcpgsr,
    CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION* pcpcs, LPWSTR*, CREDENTIAL_PROVIDER_STATUS_ICON*)
{
    if (!pcpgsr || !pcpcs)
        return E_INVALIDARG;

    *pcpgsr = CPGSR_NO_CREDENTIAL_NOT_FINISHED;
    ZeroMemory(pcpcs, sizeof(*pcpcs));
    return S_OK;
}

IFACEMETHODIMP LogonHoursCredential::ReportResult(NTSTATUS, NTSTATUS, LPWSTR*, CREDENTIAL_PROVIDER_STATUS_ICON*)
{
    return S_OK;
}
