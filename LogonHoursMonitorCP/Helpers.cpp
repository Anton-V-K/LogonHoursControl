#include "stdafx.h"

#include "Helpers.h"

#include <Shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

HRESULT FieldDescriptorCopy(const CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR& source,
    CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR** target)
{
    if (!target)
        return E_INVALIDARG;
    *target = nullptr;

    auto* copy = static_cast<CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR*>(
        CoTaskMemAlloc(sizeof(CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR)));
    if (!copy)
        return E_OUTOFMEMORY;

    copy->dwFieldID = source.dwFieldID;
    copy->cpft = source.cpft;
    copy->guidFieldType = source.guidFieldType;

    if (source.pszLabel)
    {
        const HRESULT hr = SHStrDupW(source.pszLabel, &copy->pszLabel);
        if (FAILED(hr))
        {
            CoTaskMemFree(copy);
            return hr;
        }
    }
    else
    {
        copy->pszLabel = nullptr;
    }

    *target = copy;
    return S_OK;
}
